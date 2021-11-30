#!/usr/bin/env python3.5

# Input:
#   1. libc library path
#   2. all function names and their prototypes.

# Output:
#   debloat_init.c: This is a single function, debloat_init() that needs to be
#   called from the top of the app's main.



func_to_proto = {}
func_to_size  = {}
func_dupes    = set()
funcs_called  = set()

def set_func_to_proto():
    with open("function.proto", "r") as f:
        for l in f:
            line = l.split()
            func_name = line[0][:-1]
            prototype = line[1:]
            if func_name not in func_to_proto:
                func_to_proto[func_name] = ' '.join(prototype)
            else:
                func_dupes.add(func_name)

def set_funcs_called():
    with open("functions_called_by_application2.txt", "r") as f:
        for l in f:
            lib = l.split("::")[1]
            if "libc" not in lib:
                continue
            func_name = l.split("::")[0]
            func_name = func_name.split("@")[0]
            if func_name:
                funcs_called.add(func_name)

def set_func_to_size():
    with open("libc_name_size.out", "r") as f:
        for l in f:
            line = l.split()
            if len(line) != 2:
                continue
            func_name = line[0].split("@")[0]
            size      = line[1]
            if func_name in func_to_size:
                try:
                    # take the bigger size just to be safer
                    if int(size) > int(func_to_size[func_name]):
                        func_to_size[func_name] = size
                except ValueError:
                    # noisy input file
                    pass

            func_to_size[func_name] = size


# arg example:
# debloat_init_string.format(<function_ptr_defs>,
#                            <libc_path>,
#                            <memset_strings>)
debloat_init_string = '''
void debloat_init(void)
{{
    {}
    int func_size;
    char *error;

    void *handle = dlopen("{}", RTLD_LAZY);
    if(!handle){{
      fputs(dlerror(), stderr);
      exit(1);
    }}

    {}
}}
'''


# arg eample:
# memset_string.format("glibc_strcmp", "strcmp", 47, "glibc_strcmp")
memset_string = '''
    {} = dlsym(handle, "{}");
    if((error = dlerror()) != NULL){{
        fprintf(stderr, "%s\\n", error);
        exit(1);
    }}
    func_size = {};
    memset({}, 0, func_size);
'''

# arg example:
# thunk_string.format(<func.proto>,
#                     <func.ptr_def>,
#                     <func.ptr_def_ref>,
#                     <func.size>,
#                     <func.name>,
#                     <func.name>,
#                     <func.ptr_name>,
#                     <func.name>,
#                     <func.name>,
#                     <func.ptr_name_ref>,
#                     <func.name>,
#                     <func.ptr_name>,
#                     <func.ptr_name_ref>,
#                     <func.name>,
#                     <func.ptr_name>,
#                     <func.args>)
thunk_string = '''
{}
{{
    {}
    {}
    int func_size = {};
    char *error;

    printf("thunk {}()\\n");

    if(thunk_dlopen_handle == NULL){{
        init_thunk();
    }}

    printf("thunk {}(): setting func ptr\\n");
    {} = dlsym(thunk_dlopen_handle, "{}");
    if((error = dlerror()) != NULL){{
        fprintf(stderr, "%s\\n", error);
        exit(1);
    }}
    printf("thunk {}(): setting ref func ptr\\n");
    {} = dlsym(ref_handle, "strcmp");
    if((error = dlerror()) != NULL){{
        fprintf(stderr, "%s\\n", error);
        exit(1);
    }}

    printf("thunk {}(): copying function back into place\\n");
    memcpy({}, {}, func_size);

    printf("thunk {}(): calling real version\\n");
    return {}{};
}}
'''


set_func_to_proto()
set_funcs_called()
set_func_to_size()
funcs_skipped = 0
funcs = []


class Func:
    def __init__(self, name, proto, ptr_name, ptr_def,
                 size, ptr_name_ref, ptr_def_ref, args):
        self.name = name
        self.proto     = proto
        self.ptr_name = ptr_name
        self.ptr_def  = ptr_def
        self.size     = size
        self.ptr_name_ref = ptr_name_ref
        self.ptr_def_ref = ptr_def_ref
        self.args = args

for name in funcs_called:
    if name in func_to_proto:
        proto = func_to_proto[name]
        index = proto.find(name)
        assert(index > 0) # -1 is failure. 0 is... well, that should be the return type
        ptr_name = "glibc_" + proto[index:index+len(name)]
        ptr_name_ref = "ref_glibc_" + proto[index:index+len(name)]
        ptr_def = "static " + \
                       proto[0:index] + \
                       "(*" + ptr_name + ")" + \
                       proto[index+len(name):] + \
                       " = NULL;"
        ptr_def_ref = "static " + \
                       proto[0:index] + \
                       "(*" + ptr_name_ref + ")" + \
                       proto[index+len(name):] + \
                       " = NULL;"
        if name not in func_to_size:
            print("ERROR: missing func size: %s" % name)
            exit(1)
        args = '(' + ' '.join([x.replace('*','').replace(')','').replace('(','') for x in proto[proto.find("(")+1 : ].split() if ',' in x or ')' in x and x != 'void)']) + ')' # shameless
        f = Func(name = name,
                 proto = proto,
                 ptr_name = ptr_name,
                 ptr_def  = ptr_def,
                 size = func_to_size[name],
                 ptr_name_ref = ptr_name_ref,
                 ptr_def_ref = ptr_def_ref,
                 args = args)
        funcs.append(f)
    else:
        funcs_skipped += 1


func_ptr_defs = '\n    '.join(f.ptr_def for f in funcs)
#print(func_ptr_defs)

memset_strings = []
thunk_strings = []
for f in funcs:
    ms = memset_string.format(f.ptr_name, f.name, f.size, f.ptr_name);
    memset_strings.append(ms)

    ts = thunk_string.format(f.proto,
                             f.ptr_def,
                             f.ptr_def_ref,
                             f.size,
                             f.name,
                             f.name,
                             f.ptr_name,
                             f.name,
                             f.name,
                             f.ptr_name_ref,
                             f.name,
                             f.ptr_name,
                             f.ptr_name_ref,
                             f.name,
                             f.ptr_name,
                             f.args)
    thunk_strings.append(ts)

memset_strings = '\n'.join(memset_strings)

#print(debloat_init_string.format(func_ptr_defs, "libs/libc/libc-2.23.so", memset_strings))
print('\n'.join(thunk_strings))

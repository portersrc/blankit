#define _GNU_SOURCE
#include <sys/mman.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <dlfcn.h>
#include <assert.h>
#include <unistd.h>


#define FUNC_FOO_SIZE   12
#define FUNC_FOO_OFFSET 0x610

#define RWX_PERM (PROT_READ | PROT_WRITE | PROT_EXEC)
#define RX_PERM  (PROT_READ | PROT_EXEC)


int page_size;

void *copy_page(void *addr, int size)
{
    char *aligned_addr_base;
    char *aligned_addr_end;
    int size_to_remap;

    printf("copy_page():\n");

    aligned_addr_base = (char *) ((unsigned long)(addr) & ~(page_size - 1));
    aligned_addr_end  = (char *) ((unsigned long)(addr+size) & ~(page_size - 1));
    size_to_remap = page_size + (aligned_addr_end - aligned_addr_base);
    printf("  aligned_addr_base: %p\n", aligned_addr_base);
    printf("  aligned_addr_end:  %p\n", aligned_addr_end);
    printf("  size_to_remap:     %d\n", size_to_remap);
 
    char *new_page = mmap(NULL,
                          size_to_remap,
                          PROT_READ | PROT_WRITE | PROT_EXEC,
                          MAP_ANONYMOUS | MAP_PRIVATE,
                          -1,
                          0);
    if(new_page == MAP_FAILED){
        perror("Could not mmap");
        exit(1);
    }

    memcpy(new_page, aligned_addr_base, size_to_remap);
    if(mprotect(new_page, size_to_remap, RX_PERM) == -1){
        printf("mprotect error\n");
        exit(1);
    }

    return new_page;
}



void zero_out_func_foo(void)
{
    FILE *fp;
    void *null_bytes;

    null_bytes = malloc(FUNC_FOO_SIZE);
    memset(null_bytes, 0, FUNC_FOO_SIZE);

    fp = fopen("mark_writable/libfoo.so", "r+b");

    // zero out the function
    fseek(fp, FUNC_FOO_OFFSET, 0);
    fwrite(null_bytes, FUNC_FOO_SIZE, 1, fp);
    rewind(fp);

    fclose(fp);

}


// see helper/offset_and_size.sh
//   foo 0000000000000610 12


int main(int argc, char **argv)
{
    void (*foo_handle)(void);
    void *new_mem_ptr;
    void *backup_page;
    char *foo_handle_page;
    void *dlhandle;

    printf("main\n");

    page_size = getpagesize();
    //printf("%lu\n", page_size);
    
    // This is a little hacky. We use zero_out_foo_func() to blank the
    // function, but it blanks the library file. So we need to dlopen it
    // first to copy out a pristine page.
    dlhandle = dlopen("mark_writable/libfoo.so", RTLD_LAZY);
    foo_handle = dlsym(dlhandle, "foo");
    backup_page = copy_page(foo_handle, FUNC_FOO_SIZE);
    if(dlclose(dlhandle)){
        perror("Could not dlclose");
    }

    // ... now we blank
    zero_out_func_foo();

    // dlpen libfoo, which has a blanked foo function.
    dlhandle = dlopen("mark_writable/libfoo.so", RTLD_LAZY);
    foo_handle = dlsym(dlhandle, "foo");
    foo_handle_page = (char *) ((unsigned long)(foo_handle) & ~(page_size - 1));

    printf("%p\tbackup_page     before remap\n", backup_page);
    printf("%p\tfoo_handle_page before remap\n", foo_handle_page);
    printf("%p\tfoo_handle      before remap\n", foo_handle);
    printf("\n");

    // mremap the backup page to the page with the blanked function
    new_mem_ptr = mremap(backup_page, // old address
                         page_size,   // old size
                         page_size,   // new size
                         MREMAP_FIXED | MREMAP_MAYMOVE, // flags
                         foo_handle_page); // new address

    printf("%p\tbackup_page after remap\n", backup_page);
    printf("%p\tnew_mem_ptr after remap\n", new_mem_ptr);
    printf("%p\tfoo_handle  after remap\n", foo_handle);
    printf("\n");

    // call foo, which should succeed.
    foo_handle();

    printf("done -- should see 'inside foo' printed above\n");
    return 0;
}

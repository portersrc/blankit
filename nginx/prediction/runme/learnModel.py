import time
import sys
import pandas
import numpy
import datetime
import argparse
from sklearn import tree
from sklearn.tree import DecisionTreeClassifier
from sklearn.neural_network import MLPClassifier
from sklearn.neural_network import MLPRegressor
from sklearn.preprocessing import StandardScaler  
from sklearn.preprocessing import MinMaxScaler
from sklearn.model_selection import learning_curve
from sklearn.model_selection import ShuffleSplit
from sklearn.metrics import zero_one_loss
from sklearn.metrics import mean_squared_error
from sklearn.svm import SVC
from sklearn.svm import NuSVC
from sklearn.gaussian_process.kernels import RBF
from sklearn.feature_selection import VarianceThreshold
from sklearn.metrics import roc_curve
from sklearn.metrics import auc
#from sklearn import ensemble
from sklearn.ensemble import RandomForestClassifier, AdaBoostClassifier, GradientBoostingClassifier
from sklearn.neighbors import KNeighborsClassifier
from sklearn.feature_selection import VarianceThreshold
from sklearn.model_selection import validation_curve
import matplotlib.pyplot as plt
import matplotlib
import pickle
from sklearn.externals import joblib


names = [ 
         "Neural Net", 
         "AdaBoost",
"Nearest Neighbors",
"RBF SVM", 
         "Decision Tree", 
#"Random Forest", 
         "Linear SVM"
         ]

classifiers = [
    MLPClassifier(hidden_layer_sizes=(50,50), learning_rate='adaptive'),
    AdaBoostClassifier( ) , 
    KNeighborsClassifier(10, weights='distance' ),
    SVC( probability=True,C=1, max_iter=5000),
    DecisionTreeClassifier(min_impurity_split=0.3, max_depth=10 ),
    #RandomForestClassifier( ),
    SVC(probability=True,kernel="linear", max_iter=500)
    ]

plot_markers = ["o-", "v-", "x-", "D-", "+-", "*"]
def plot_learning_curve(estimator, title, X, y, ylim=None , 
cv=None,
                        n_jobs=6, train_sizes=numpy.linspace(.1, 1.0, 5), last_plot=None, continue_last=0,
                        algo_name="", given_marker="o-" ):
    """
    Generate a simple plot of the test and training learning curve.

    Parameters
    ----------
    estimator : object type that implements the "fit" and "predict" methods
        An object of that type which is cloned for each validation.

    title : string
        Title for the chart.

    X : array-like, shape (n_samples, n_features)
        Training vector, where n_samples is the number of samples and
        n_features is the number of features.

    y : array-like, shape (n_samples) or (n_samples, n_features), optional
        Target relative to X for classification or regression;
        None for unsupervised learning.

    ylim : tuple, shape (ymin, ymax), optional
        Defines minimum and maximum yvalues plotted.

    cv : int, cross-validation generator or an iterable, optional
        Determines the cross-validation splitting strategy.
        Possible inputs for cv are:
          - None, to use the default 3-fold cross-validation,
          - integer, to specify the number of folds.
          - An object to be used as a cross-validation generator.
          - An iterable yielding train/test splits.

        For integer/None inputs, if ``y`` is binary or multiclass,
        :class:`StratifiedKFold` used. If the estimator is not a classifier
        or if ``y`` is neither binary nor multiclass, :class:`KFold` is used.

        Refer :ref:`User Guide <cross_validation>` for the various
        cross-validators that can be used here.

    n_jobs : integer, optional
        Number of jobs to run in parallel (default 1).
    """
    if continue_last == 0:
        plt.figure()
        plt.title(title)
        if ylim is not None:
            plt.ylim(*ylim)
        plt.xlabel("Training examples")
        plt.ylabel("Score")
        plt.grid()
        #plt.xscale('log')
    train_sizes, train_scores, test_scores = learning_curve(
        estimator, X, y, cv=cv, n_jobs=n_jobs, train_sizes=train_sizes)
    train_scores_mean = numpy.mean(train_scores, axis=1)
    train_scores_std = numpy.std(train_scores, axis=1)
    test_scores_mean = numpy.mean(test_scores, axis=1)
    test_scores_std = numpy.std(test_scores, axis=1)

    plt.fill_between(train_sizes, train_scores_mean - train_scores_std,
                     train_scores_mean + train_scores_std, alpha=0.1,
                     color="r")
    plt.fill_between(train_sizes, test_scores_mean - test_scores_std,
                     test_scores_mean + test_scores_std, alpha=0.1, color="g")
    label_s="Training score "+ algo_name
    marker = given_marker
    plt.plot(train_sizes, train_scores_mean, marker, label=label_s )
    label_s="Cross-validation score "+ algo_name
    plt.plot(train_sizes, test_scores_mean, marker,
             label=label_s)

    plt.legend(loc="best")
    if save_plots:
        filename=title.replace(" ","_")+".png"
        plt.savefig(filename )
    #else:
      #  plt.show()
    return plt

def read_csv_get_dataframe(csvFilename ) :
    print 'Working with csv file::' + csvFilename
    csvData     = pandas.read_csv( csvFilename )
    return csvData
    #csvData_flat = pandas.get_dummies(csvData)#/Convert categorical variable into dummy indicator/bool columns
    #csvData = csvData.applymap(lambda x: x.is_numeric())
    #csvData = csvData.applymap(lambda x: ord(x))
    #csvData = csvData.applymap(lambda x: x if type(x) in [int, numpy.int64, float, numpy.float64] else ord(x) )
#if there are any chars replace by number
    return csvData_flat

def rand_sample_dataset(csvData, split_frac ) :
    #sel = VarianceThreshold(threshold=(.8 * (1 - .8)))
    #csvData.values = sel.fit_transform(csvData.values)
    numTraining = int(split_frac *csvData.shape[0])
    print "Number of training samples:"+str(numTraining)
    dataset = csvData.sample(numTraining )
    testData = csvData.drop(dataset.axes[0] )
    #print "\n Saved train and test dataset to train.csv and test.csv"
    #dataset.to_csv("train.csv" )
    #testData.to_csv("test.csv" )
    return dataset, testData
def swap_panda_frame(df, col_from,col_to ) :
# get a list of the columns
    col_list = list(df)
    cols = df.columns.tolist()
    #cols = cols[-1:] + cols[:-1]
    #df = df[cols]
# use this handy way to swap the elements
    col_list[col_to], col_list[col_from] = col_list[col_from], col_list[col_to]
# assign back, the order will now be swapped
    df.columns = col_list
    print "\n After swapping"
    print df


def LearndecisionTreeClassifier(trainX,trainY,testX,testY):
    decisionTreeClf = DecisionTreeClassifier( presort=True, max_depth=max_tree_depth )
    decisionTreeClf = decisionTreeClf.fit(trainX,trainY )

    if save_plots != "":
        graphName = save_plots + ".pkl"
        joblib.dump(decisionTreeClf, graphName)
    accuracy = decisionTreeClf.score(testX, testY)
    predicrtedY = decisionTreeClf.predict(testX)
    pandas.DataFrame(predicrtedY).to_csv('prediction.csv')
    #numpy.savetxt('predicted.csv', predicrtedY)
    #with pandas.option_context('display.max_rows', None):#, 'display.max_columns', 3):
    #        print(predicrtedY)
    #        print(testY)
    #        print(trainY)
    #        print(trainX)
    #        print(testX)

    print "\n Accuracy = "+str(accuracy)
    print "\n Depth = "+str(decisionTreeClf.tree_.max_depth )
    print "\n decision tree path::\n"+str(decisionTreeClf.decision_path(trainX[2:3,:]))
    print "\n decision tree ::\n"+str(decisionTreeClf.n_classes_)
    listClassNames = []
    for cl in range(0,decisionTreeClf.n_classes_):
        listClassNames.append(str(cl))

    if save_plots != "":
        graphName = save_plots + ".dot"
        with open(graphName, 'w') as f:
            f = tree.export_graphviz(decisionTreeClf, out_file=f, feature_names=list(training_dataset)[1:],
             class_names=listClassNames)
             #('0','1','2','3','4','5','6','7','8','9','10','11','12','13','14','15','16','17') )
             #)
     
    #allDatasetX=numpy.concatenate((trainX,testX ))
    #allDatasetY=numpy.concatenate((trainY,testY ))
    #allDatasetX=trainX
    #allDatasetY=trainY
    #plot_learning_curve(decisionTreeClf,"Learning Curve with Decision Tree",allDatasetX,allDatasetY)
    #param_range = [1, 2,  7, 8, 9, 10, 13, 14, 15, 16, 20, 30, 40]
    #plot_validation_curve(decisionTreeClf, allDatasetX, allDatasetY, "max_depth",param_range , "Decision Tree")
    #param_range = [0.2,0.3,0.4,0.5,0.6,0.7,0.8,0.9,0.99]
    #plot_validation_curve(decisionTreeClf, allDatasetX, allDatasetY, "min_samples_split",param_range , "Decision Tree")
    #param_range = [0.2,0.3,0.4,0.5]
    #plot_validation_curve(decisionTreeClf, allDatasetX, allDatasetY, "min_samples_leaf",param_range , "Decision Tree")
    #param_range = [0.00005, 0.005, 0.1,0.2,0.3,0.4,0.5,0.6, 0.7,0.8]
    #plot_validation_curve(decisionTreeClf, allDatasetX, allDatasetY, "min_impurity_split",param_range , "Decision Tree")
    #return
    #plotRUC(decisionTreeClf, testX,testY, "Decision Tree")


def supervisedLearn(training_dataset, test_dataset,split_frac ):
    trainX = training_dataset.values[:,1:] # First column is the output to be estimated
    #convert_category_to_float(trainX )
    trainY = training_dataset.values[:,0]
# Don't cheat - fit only on training data
# apply same transformation to test data
    testX = test_dataset.values[:,1:]
    testY = test_dataset.values[:,0]
    #a = numpy.asarray(trainY)
    #numpy.savetxt("trainY.csv",a,delimiter="," )
    #a = numpy.asarray(testY)
    #numpy.savetxt("testY.csv",a,delimiter="," )
    #return
    if do_scaling == 1:
        scaler = StandardScaler()  
        scaler.fit(trainX)  
        trainX = scaler.transform(trainX)  
        testX = scaler.transform(testX)  
    LearndecisionTreeClassifier(trainX,trainY,testX,testY )
    print "\n Done Decision Tree"
    return

if __name__ == '__main__' :
    #font = {'family':'normal' , 'weight' : 'bold', 'size' : 22 }
    #matplotlib.rc('font',**font )
#Init End
    parser = argparse.ArgumentParser(description='learning algorithm selector' )
    parser.add_argument('-csv_file_name',dest='csv_file_name', type=str,required=True,  help='Data File in CSV Format')
    parser.add_argument('-test_csv_file_name',dest='test_csv_file_name', type=str,required=False,  help='Output Data File in CSV Format',default=None)
    parser.add_argument('-split_frac',dest='split_frac', type=float,required=False,  help='Split of Test and Train',default=1.0)
    parser.add_argument('-output_col',dest='output_col', type=int,required=False ,  help='Column of Output')
    parser.add_argument('-only_decisiontree',dest='only_decisiontree', type=int,required=False ,  help='Only Decision Tree',default=0)
    parser.add_argument('-save_plots',dest='save_plots', type=str,required=False,  help='Save plots to file', default="")
    parser.add_argument('-do_scaling',dest='do_scaling', type=int,required=False ,  help='Normalize input data',default=0)
    parser.add_argument('-max_tree_depth',dest='max_tree_depth', type=int,required=False ,  help='Normalize input data',default=50)
    args = parser.parse_args()
    csvFileName= args.csv_file_name
    if args.test_csv_file_name is None:
        test_csvFileName = args.csv_file_name
    else: 
        test_csvFileName= args.test_csv_file_name
    split_frac= args.split_frac
    only_decisiontree = args.only_decisiontree
    save_plots = args.save_plots
    do_scaling = args.do_scaling
    max_tree_depth= args.max_tree_depth
    output_col = args.output_col
#Init End

    dataframe_fromcsv= read_csv_get_dataframe(csvFileName )
    test_dataframe_fromcsv= read_csv_get_dataframe(test_csvFileName )
    #print dataframe_fromcsv
    #print dataframe_fromcsv.shape
    #training_dataset, test_dataset = rand_sample_dataset(dataframe_fromcsv, split_frac )
    training_dataset, test_dataset = dataframe_fromcsv, test_dataframe_fromcsv
    #cols = training_dataset.columns.tolist()
    #cols = cols[-1:] + cols[:-1]
    #training_dataset = training_dataset[cols]
    #swap_panda_frame(training_dataset, args.output_col ,0)
    #swap_panda_frame(test_dataset, args.output_col ,0)
    #print "\n Dataset headers::"
    #print "\n training::"
    #print  training_dataset
    #print "\n test ::"
    #print test_dataset
    #print list( training_dataset)
    #print list( test_dataset)
    #print test_dataset.shape
    supervisedLearn(training_dataset, test_dataset,split_frac )
    #plt.show()

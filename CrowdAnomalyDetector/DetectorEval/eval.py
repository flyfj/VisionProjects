
import sklearn
import sklearn.metrics
import matplotlib.pyplot as plt

def comp_roc(res_fn, label_fn):
    res_scores = []
    with open(res_fn, 'r') as res:
        num = int(res.readline())
        for i in range(num):
            res_scores.append( float(res.readline()) )

    labels = []
    with open(label_fn, 'r') as f:
        num = int(f.readline())
        for i in range(num):
            labels.append( float(f.readline()) )

    # make sure same dimension
    valid_num = min(len(res_scores), len(labels))
    res_scores = res_scores[:valid_num]
    labels = labels[:valid_num]
    # compute roc
    fpr, tpr, thresholds = sklearn.metrics.roc_curve(labels, res_scores, 1)
    roc_auc = sklearn.metrics.auc(fpr, tpr)
    # plot
    plt.figure()
    plt.title('ROC curve (area = {0:0.2f})'.format(roc_auc))
    plt.plot(fpr, tpr)
    plt.show()


if __name__ == '__main__':
    comp_roc('outputs.txt', 'labels.txt')

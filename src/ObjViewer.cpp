/** \file
 *
 *  \brief Implement ObjViewer window.
 *
 */

#include "ObjViewer.hpp"

ObjDLModel::ObjDLModel(std::map<size_t, RCP::DisplayList> * dm) : dlmod(dm) { }

int ObjDLModel::rowCount(const QModelIndex & /*parent*/) const {
    return dlmod->size();
}

QVariant ObjDLModel::data(const QModelIndex & index, int role) const {
    if (role == Qt::DisplayRole) {
        return QString("0x%1").arg(
            QString("%1").arg(
                std::next(dlmod->begin(), index.row())->first,
                0,
                16,
                QChar('0')).toUpper());
    }

    return QVariant();
}


ObjViewer::ObjViewer(ROM::File rf, QWidget * parent) : QWidget(parent) {
    dl_map = RCP::getDLs(rf.begin(), rf.end());

    odlm = new ObjDLModel(&dl_map);
    dl_list = new QListView(this);
    dl_list->setModel(odlm);
    dl_list->setSelectionMode(QAbstractItemView::SingleSelection);

    connect(dl_list->selectionModel(), &QItemSelectionModel::currentChanged,
            this, &ObjViewer::selectItem);

    qte = new QTextEdit;

    qhb = new QHBoxLayout;
    qhb->addWidget(dl_list);
    qhb->addWidget(qte);

    setLayout(qhb);
    setWindowTitle("Object Viewer");
}

void ObjViewer::selectItem(const QModelIndex & cur, const QModelIndex & /*prev*/) {
    size_t addr = std::next(dl_map.begin(), cur.row())->first;

    qte->clear();

    for (auto & i : dl_map.at(addr)) {
        qte->append(i->id().c_str());
    }
}
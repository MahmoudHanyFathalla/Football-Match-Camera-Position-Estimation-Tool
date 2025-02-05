
#include "shortcutsdialog.h"
#include "qstandarditemmodel.h"
#include "ui_shortcutsdialog.h"

ShortcutsDialog::ShortcutsDialog(QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::ShortcutsDialog)
{
    initializeShortcutList();

    ui->setupUi(this);
    QStandardItemModel *model = new QStandardItemModel(shortcuts.size(), 2);
    for (int row = 0; row < shortcuts.size(); ++row) {
        for (int column = 0; column < 2; ++column) {
            QStandardItem *item = new QStandardItem(shortcuts[row][column]);
            model->setItem(row, column, item);
        }
    }
    ui->tableView->setModel(model);
    ui->tableView->setEditTriggers(QAbstractItemView::NoEditTriggers);
    ui->tableView->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);

}

ShortcutsDialog::~ShortcutsDialog()
{
    delete ui;
}



void ShortcutsDialog::initializeShortcutList()
{
    QList<QString> innerList;
    innerList.append("Next img");
    innerList.append("D");
    shortcuts.emplaceBack(innerList);
    innerList.clear();

    innerList.append("Previous img");
    innerList.append("A");
    shortcuts.emplaceBack(innerList);
    innerList.clear();

    innerList.append("save img");
    innerList.append("Enter");
    shortcuts.emplaceBack(innerList);
    innerList.clear();

    innerList.append("rotate vertically to left <-");
    innerList.append("V");
    shortcuts.emplaceBack(innerList);
    innerList.clear();

    innerList.append("rotate vertically to Right ->");
    innerList.append("N");
    shortcuts.emplaceBack(innerList);
    innerList.clear();

    innerList.append("rotate horizontally to left <-");
    innerList.append("I");
    shortcuts.emplaceBack(innerList);
    innerList.clear();

    innerList.append("rotate horizontally to Right ->");
    innerList.append("P");
    shortcuts.emplaceBack(innerList);
    innerList.clear();

    innerList.append("zoom on or off");
    innerList.append("Z");
    shortcuts.emplaceBack(innerList);
    innerList.clear();

    innerList.append("look on or off");
    innerList.append("L");
    shortcuts.emplaceBack(innerList);
    innerList.clear();

    innerList.append("Refersh");
    innerList.append("R");
    shortcuts.emplaceBack(innerList);
    innerList.clear();

    innerList.append("Full img 1");
    innerList.append("1");
    shortcuts.emplaceBack(innerList);
    innerList.clear();

    innerList.append("Full img 2");
    innerList.append("2");
    shortcuts.emplaceBack(innerList);
    innerList.clear();

    innerList.append("Full img 3");
    innerList.append("3");
    shortcuts.emplaceBack(innerList);
    innerList.clear();

    innerList.append("Full img 4");
    innerList.append("4");
    shortcuts.emplaceBack(innerList);
    innerList.clear();

    innerList.append("All imgs");
    innerList.append("5");
    shortcuts.emplaceBack(innerList);
    innerList.clear();
/*
    innerList.append("remove down slider");
    innerList.append("6");
    shortcuts.emplaceBack(innerList);
    innerList.clear();

    innerList.append("get back down slider");
    innerList.append("7");
    shortcuts.emplaceBack(innerList);
    innerList.clear();

    innerList.append("remove right slider");
    innerList.append("8");
    shortcuts.emplaceBack(innerList);
    innerList.clear();

    innerList.append("get back right slider");
    innerList.append("9");
    shortcuts.emplaceBack(innerList);
    innerList.clear();
*/
    innerList.append("Night mood or light mood");
    innerList.append("m");
    shortcuts.emplaceBack(innerList);
    innerList.clear();

    innerList.append("increase moving ratio");
    innerList.append("+");
    shortcuts.emplaceBack(innerList);
    innerList.clear();

    innerList.append("decrease moving ratio");
    innerList.append("-");
    shortcuts.emplaceBack(innerList);
    innerList.clear();

    innerList.append("move point to left");
    innerList.append("<-");
    shortcuts.emplaceBack(innerList);
    innerList.clear();

    innerList.append("move point to right");
    innerList.append("->");
    shortcuts.emplaceBack(innerList);
    innerList.clear();

    innerList.append("move point to up");
    innerList.append("↑");
    shortcuts.emplaceBack(innerList);
    innerList.clear();

    innerList.append("move point to down");
    innerList.append("↓");
    shortcuts.emplaceBack(innerList);
    innerList.clear();
}

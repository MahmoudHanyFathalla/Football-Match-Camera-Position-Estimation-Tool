#ifndef SHORTCUTSDIALOG_H
#define SHORTCUTSDIALOG_H

#include <QDialog>
#include <QList>

namespace Ui {
class ShortcutsDialog;
}

class ShortcutsDialog : public QDialog
{
    Q_OBJECT

public:
    explicit ShortcutsDialog(QWidget *parent = nullptr);
    ~ShortcutsDialog();

    void initializeShortcutList();
private:
    QList<QList<QString>> shortcuts;
    Ui::ShortcutsDialog *ui;
};

#endif // SHORTCUTSDIALOG_H

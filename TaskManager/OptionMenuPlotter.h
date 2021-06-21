#ifndef OPTIONMENUPLOTTER_H
#define OPTIONMENUPLOTTER_H

#include <QPen>
#include <QWidget>

namespace Ui {
class OptionMenuPlotter;
}

class SettingsChart
{
public:
    QColor Background, Cursor;
    QPen GraphColor;
    QFont GraphFont;
    void clear();
};

class OptionMenuPlotter : public QWidget
{
    Q_OBJECT

public:
    explicit OptionMenuPlotter(QWidget *parent = nullptr);
    ~OptionMenuPlotter();
    SettingsChart ChartToSet;
    bool InProgress=true;

private slots:
    void on_select_color_btn_clicked();
    void on_ok_btn_clicked();
    void on_cancel_btn_clicked();
    void on_font_box_currentFontChanged(const QFont &f);
    void on_graph_rd_btn_clicked();
    void on_background_rd_btn_clicked();
    void on_cursor_rd_btn_clicked();

private:
    Ui::OptionMenuPlotter *ui;
    int m_SelectColorToSet{};

};



#endif // OPTIONMENUPLOTTER_H

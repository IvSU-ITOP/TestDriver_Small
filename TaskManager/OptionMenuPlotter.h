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
    bool isChange = false;
    QColor Background,BackgroundGraph, Cursor;
    QColor GraphColor,GridLine, AxisColorX,AxisColorY;
    QFont GraphFont;
    int ThinknessAxisX{3},ThinknessAxisY{3},Thinkness;
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
    void on_thinkness_valueChanged(const QString &arg1);
    void on_object_to_set_currentIndexChanged(int index);

signals:
    void sendDataClass(void);
private:
    Ui::OptionMenuPlotter *ui;
    int m_SelectColorToSet{};

};



#endif // OPTIONMENUPLOTTER_H

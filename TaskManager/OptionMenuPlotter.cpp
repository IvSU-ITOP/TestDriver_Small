#include "OptionMenuPlotter.h"
#include "ui_OptionMenuPlotter.h"
#include <QColorDialog>

OptionMenuPlotter::OptionMenuPlotter(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::OptionMenuPlotter)
{
    ui->setupUi(this);
    ChartToSet.clear();
}

OptionMenuPlotter::~OptionMenuPlotter()
{
    delete ui;
}

void OptionMenuPlotter::on_select_color_btn_clicked()
{
        QColor color = QColorDialog::getColor();
        if (color.isValid() )
        {
            switch (m_SelectColorToSet)
            {
                case 0:{ChartToSet.Background=color;break;}
                case 1:{ChartToSet.BackgroundGraph=color;break;}
                case 2:{ChartToSet.GraphColor=color;break;}
                case 3:{ChartToSet.GridLine=color;break;}
                case 4:{ChartToSet.Cursor=color;break;}
                default:break;
            }
        }
}


void OptionMenuPlotter::on_ok_btn_clicked()
{
    ChartToSet.isChange=true;
    emit sendDataClass();
    this->hide();
}


void OptionMenuPlotter::on_cancel_btn_clicked()
{
    ChartToSet.clear();
}


void OptionMenuPlotter::on_font_box_currentFontChanged(const QFont &f)
{
    ChartToSet.GraphFont=f;
}


void OptionMenuPlotter::on_thinkness_valueChanged(const QString &arg1)
{
    ChartToSet.Thinkness=arg1.toInt();
}

void SettingsChart::clear()
{
    isChange=false;
    Thinkness=3;
    Background=QColor("white");
    BackgroundGraph=QColor("white");
    Cursor=QColor("red");
    GraphColor=QColor("black");
    GraphFont=QFont("Helvetica");
    GridLine=QColor("black");
}

void OptionMenuPlotter::on_object_to_set_currentIndexChanged(int index)
{
    m_SelectColorToSet=index;
}


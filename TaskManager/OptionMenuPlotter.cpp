#include "OptionMenuPlotter.h"
#include "ui_optionmenuplotter.h"
#include <QColorDialog>

OptionMenuPlotter::OptionMenuPlotter(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::OptionMenuPlotter)
{
    ui->setupUi(this);
}

OptionMenuPlotter::~OptionMenuPlotter()
{
    delete ui;
}

void OptionMenuPlotter::on_select_color_btn_clicked()
{
    if(m_SelectColorToSet!=0)
    {
        QColor color = QColorDialog::getColor();
        if (color.isValid() )
        {
            switch (m_SelectColorToSet)
            {
                case 1:{ChartToSet.GraphColor=QPen(color);break;}
                case 2:{ChartToSet.Background=color;break;}
                case 3:{ChartToSet.Cursor=color;break;}
                default:break;
            }
        }
    }
}


void OptionMenuPlotter::on_ok_btn_clicked()
{
    if(InProgress)
    {
    InProgress=false;
    }
}


void OptionMenuPlotter::on_cancel_btn_clicked()
{
    ChartToSet.clear();
}


void OptionMenuPlotter::on_font_box_currentFontChanged(const QFont &f)
{
    ChartToSet.GraphFont=f;
}


void OptionMenuPlotter::on_graph_rd_btn_clicked()
{
    m_SelectColorToSet=1;
}


void OptionMenuPlotter::on_background_rd_btn_clicked()
{
    m_SelectColorToSet=2;
}


void OptionMenuPlotter::on_cursor_rd_btn_clicked()
{
    m_SelectColorToSet=3;
}


void SettingsChart::clear()
{
    Background=QColor("black");
    Cursor=QColor("black");
    GraphColor=QPen("black");
    GraphFont=QFont("Helvetica");
}

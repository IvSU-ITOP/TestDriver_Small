#include "OptionMenuPlotter.h"
#include "ui_OptionMenuPlotter.h"
#include <QColorDialog>
#include <QPainter>

OptionMenuPlotter::OptionMenuPlotter(int FuncNumber,QWidget *parent) :
    QWidget(parent),
    ui(new Ui::OptionMenuPlotter)
{
    ui->setupUi(this);
    ChartToSet.clear();
    HideAllSettings();
    QGraphicsScene* Scene=new QGraphicsScene();
    ui->graphicsView->setScene(Scene);
    if(FuncNumber!=0)
    {
        for(int i{};i<=FuncNumber;i++)
        {
           ui->comboBox->addItem(QString::number(i));
        }
    }
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
                case 4:{ChartToSet.BreakPointColor=color;break;}
                case 5:{ChartToSet.AxisColorX=color;break;}
                case 6:{ChartToSet.AxisColorY=color;break;}
                default:break;
            }
            ui->graphicsView->setBackgroundBrush(QBrush(color));
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
    this->hide();
}


void OptionMenuPlotter::on_font_box_currentFontChanged(const QFont &f)
{
    switch (m_SelectColorToSet)
    {
    case 5:{ChartToSet.FontAxisY=f;break;}
    case 6:{ChartToSet.FontAxisX=f;break;}
    default:{ChartToSet.GraphFont=f; break;}
    }
}


void OptionMenuPlotter::on_thinkness_valueChanged(const QString &arg1)
{
    switch (m_SelectColorToSet)
    {
    case 4:{ChartToSet.ThinknessBreakPoint=arg1.toInt();break;}
    case 5:{ChartToSet.ThinknessAxisX=arg1.toInt();break;}
    case 6:{ChartToSet.ThinknessAxisY=arg1.toInt();break;}
    default: {ChartToSet.ThinknessGraph=arg1.toInt(); break;}
    }
}

void SettingsChart::clear()
{
    isChange=false;
    ThinknessGraph=3;
    ThinknessAxisX=1;
    ThinknessAxisY=1;
    ThinknessBreakPoint=8;
    Background=QColor("white");
    BackgroundGraph=QColor("white");
    BreakPointColor=QColor("red");
    GraphColor=QColor("black");
    AxisColorX=QColor("Grey");
    AxisColorY=QColor("Grey");
    GraphFont=QFont("Arial",8);
    FontAxisX=QFont("Arial",8,5);
    FontAxisY=QFont("Arial",8,5);
    GraphPen.setColor(GraphColor);
    GraphPen.setWidth(ThinknessGraph);
}

void OptionMenuPlotter::on_object_to_set_currentIndexChanged(int index)
{
    m_SelectColorToSet=index;
    HideAllSettings();
    switch (m_SelectColorToSet)
    {
        case 0:{ui->graphicsView->setBackgroundBrush(QBrush(ChartToSet.Background));break;}
        case 1:{ui->graphicsView->setBackgroundBrush(QBrush(ChartToSet.BackgroundGraph));break;}
        case 2:
        {
        ui->graphicsView->setBackgroundBrush(QBrush(ChartToSet.GraphColor));
        ui->font->setText("Font of title Graph");
        break;
        }
        case 4:
        {
        ui->graphicsView->setBackgroundBrush(QBrush(ChartToSet.BreakPointColor));
        ui->thinkness->setMinimum(5);
        ui->thinkness->setMaximum(10);
        break;
        }
        case 5:
        {
        ui->graphicsView->setBackgroundBrush(QBrush(ChartToSet.AxisColorX));
        ui->font->setText("Font of title Axis X");
        break;
        }
        case 6:
        {
        ui->graphicsView->setBackgroundBrush(QBrush(ChartToSet.AxisColorY));
        ui->font->setText("Font of title Axis Y");
        break;
        }
        default:break;
    }
    if(m_SelectColorToSet==5 ||m_SelectColorToSet==6)
    {
        ui->thinkness->show();
        ui->label_thinkness->show();
        ui->font->show();
        ui->font_box->show();
        ui->pixel->show();
     }
    if(m_SelectColorToSet==2 || m_SelectColorToSet==4)
    {
        ui->fx->show();
        ui->comboBox->show();
        ui->thinkness->show();
        ui->label_thinkness->show();
        ui->font->show();
        ui->font_box->show();
        ui->pixel->show();
    }
}

void OptionMenuPlotter::HideAllSettings()
{
    ui->thinkness->setMinimum(1);
    ui->thinkness->setMaximum(5);
    ui->thinkness->hide();
    ui->label_thinkness->hide();
    ui->font->hide();
    ui->font_box->hide();
    ui->pixel->hide();
    ui->fx->hide();
    ui->comboBox->hide();
    ui->retranslateUi(this);
}



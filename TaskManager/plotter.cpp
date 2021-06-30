#include "Plotter.h"

#include <OptionMenuPlotter.h>
#include <Parser.h>
#include <ui_Plotter.h>

Plotter::Plotter(QObject *parent)
   : QMainWindow(nullptr)
{
  m_pUi=new Ui::Plotter;
  m_pUi->setupUi(this);
  m_pUi->PlotterWidget->setContextMenuPolicy(Qt::CustomContextMenu);
  m_pScene=new QGraphicsScene(m_pUi->graphicsView);
  connect(m_pUi->PlotterWidget, &QWidget::customContextMenuRequested, this, &Plotter::on_ContextMenuCall);
  connect(m_pPlotterMenu,&OptionMenuPlotter::sendDataClass,this,&Plotter::on_SetChartSettings);

  m_pValueAxisX->setTitleText("X");
  m_pValueAxisY->setTitleText("Y");
  m_pValueAxisX->setTickCount(20);
  m_pValueAxisY->setTickCount(20);
  m_pValueAxisX->setGridLineVisible(false);
  m_pValueAxisY->setGridLineVisible(false);
  m_pValueAxisX->applyNiceNumbers();
  m_pValueAxisY->applyNiceNumbers();

  m_pCursor->setColor(Qt::blue);
  m_pCursor->setBorderColor(Qt::blue);
  m_pCursor->setMarkerSize(8);
  QPen PenLine(Qt::blue);
  PenLine.setWidth(1);
  m_pLinesCursor->setPen(PenLine);
  m_pChart->addSeries(m_pCursor);
  m_pChart->addSeries(m_pAxisX);
  m_pChart->addSeries(m_pAxisY);
  m_pChart->addSeries(m_pLinesCursor);

  m_pMainChart->clear();
  m_pSeries0->setPen(m_pMainChart->GraphPen);
  m_pSeriesBreakPoints->setBorderColor(m_pMainChart->BreakPointColor);
  m_pSeriesBreakPoints->setColor(Qt::white);
  m_pSeriesBreakPoints->setMarkerSize(qreal(m_pMainChart->ThinknessBreakPoint));

  m_pAxisX->setPointLabelsFormat("@xPoint");
  m_pAxisY->setPointLabelsFormat("@yPoint");

  m_pChart->setGeometry( m_pUi->graphicsView->rect());
  m_pChart->createDefaultAxes();
  m_pChart->axisX()->hide();
  m_pChart->axisY()->hide();
  m_pChart->addAxis(m_pValueAxisX,Qt::AlignBottom);
  m_pChart->addAxis(m_pValueAxisY,Qt::AlignLeft);
  m_pChart->legend()->hide();
  m_pValueAxisX->hide();
  m_pValueAxisY->hide();
}


Plotter::~Plotter()
{
  delete m_pUi;
}

QVector <QPointF> Plotter::CalculatePoint()
{
 if( m_Formula.isEmpty() || m_Formula.contains("/0") )return {};

  m_Formula=m_Formula.toLower();

 if(m_Formula.contains("y") && m_Formula[m_Formula.indexOf('y')+1]=='=') m_Formula.remove(m_Formula.indexOf('y'),2);
 if(!m_Formula.contains('x') && !m_Formula.contains('y')) return {};

 QVector <QPointF> Result;
 double Y{};
 double X_start(m_pUi->xmin->value()), X_end(m_pUi->xmax->value()), X_step(0.01);

  for( double X = X_start; ceil(X) <= X_end; X += X_step)
  {
   if(fabs(X) < 0.5 * X_step ) X = 0;

    QByteArray Formula_copy(m_Formula);
    QByteArray TextX(QByteArray::number(X));
    if(X < 0) TextX = '(' + TextX + ')';
    Formula_copy.remove(m_Formula.indexOf("exp"),3);
    if(m_Formula.contains('x')) Formula_copy.replace('x', TextX);
    if(m_Formula.contains('y')) Formula_copy.replace('y', TextX);
    Formula_copy.insert(m_Formula.indexOf("exp"),m_Formula.mid(m_Formula.indexOf("exp"),3));
    MathExpr Expr = MathExpr( Parser::StrToExpr( Formula_copy));

    if(s_GlobalInvalid || Expr.IsEmpty())return {};

    Expr=Expr.SimplifyFull();

    if(s_GlobalInvalid && s_LastError=="INFVAL" && !Result.isEmpty())
        m_BreakPoints.append(QPointF(X, Y));

    TConstant *pValue =CastPtr(TConstant, Expr);
    if(!(pValue==nullptr) && !s_GlobalInvalid)
    {
      if(pValue->IsLimit()) m_BreakPoints.append(QPointF(X, pValue->Value()));
      else
      {
        Y = pValue->Value();
        if(Y < m_YMin) m_YMin = Y;
        if( Y > m_YMax ) m_YMax = Y;
        Result.append(QPointF(X, Y));
      }
    }
   }
   m_pUi->cur_val_slider->setMaximum(Result.length());
   return Result;
 }

bool Plotter::Plot(QByteArray Formula)
{
 m_Formula=Formula;
 m_Result=CalculatePoint();

 m_pValueAxisX->setRange(m_pUi->xmin->value(),m_pUi->xmax->value());
 if(m_YMin==m_YMax){m_YMax++;m_YMin--;}
 m_pValueAxisY->setRange(floor(m_YMin), ceil(m_YMax) );

 m_pUi->ymin->blockSignals(true);
 m_pUi->ymax->blockSignals(true);
 m_pUi->ymin->setValue( floor(m_YMin) );
 m_pUi->ymax->setValue( ceil(m_YMax) );
 m_pUi->ymin->blockSignals(false);
 m_pUi->ymax->blockSignals(false);

 if(m_Result.isEmpty())
 {
   this->~Plotter();
   return false;
 }
 m_pChart->addSeries(m_pSeriesBreakPoints);
 m_pChart->addSeries(m_pSeries0);

 m_pChart->setTitle("Y");
 QChartView *chartView = new QChartView(m_pChart);
 chartView->setRenderHint(QPainter::Antialiasing);
 m_pScene->addItem(m_pChart);
 m_pUi->graphicsView->setScene(m_pScene);
 UpdateGraph();
 return true;
}

void Plotter::ReCalculateAndUpdate()
{
  m_Result.clear();
  m_BreakPoints.clear();
  s_LastError="";
  m_Result=CalculatePoint();
  UpdateGraph();
}

void Plotter::UpdateGraph()
{
  m_pAxisX->clear();
  m_pAxisY->clear();
  m_pCursor->clear();
  m_pLinesCursor->clear();
  m_pLabelCursor->hide();

  m_pAxisX->attachAxis(m_pValueAxisX);
  m_pAxisY->attachAxis(m_pValueAxisY);
  m_pAxisX->setPen(m_pMainChart->AxisXPen);
  m_pAxisY->setPen(m_pMainChart->AxisYPen);
  m_pAxisX->setPointLabelsFont((m_pMainChart->FontAxisX));
  m_pAxisY->setPointLabelsFont((m_pMainChart->FontAxisY));


  int div=ceil( abs(m_pUi->xmax->value()))/5 ;
  for(int LabelPoint=m_pUi->xmin->value();LabelPoint<m_pUi->xmax->value() && div!=0;LabelPoint+=div)
  {
    m_pAxisX->append(QPointF(LabelPoint,0));
  }
  div=ceil( abs(m_pUi->ymax->value()))/5 ;
  for(int LabelPoint=m_pUi->ymin->value();LabelPoint<m_pUi->ymax->value() && div!=0;LabelPoint+=div)
  {
    m_pAxisY->append(QPointF(0,LabelPoint));
  }
  m_pAxisX->append(QPointF(m_pUi->xmin->value(),0));
  m_pAxisX->append(QPointF(m_pUi->xmax->value(),0));
  m_pAxisY->append(QPointF(0, m_pUi->ymin->value()));
  m_pAxisY->append(QPointF(0, m_pUi->ymax->value()));

  m_pAxisX->setPointLabelsVisible(true);
  m_pAxisY->setPointLabelsVisible(true);

  m_pSeries0->attachAxis(m_pValueAxisY);
  m_pSeries0->attachAxis(m_pValueAxisX);
  m_pSeries0->replace(m_Result);

  if(!m_BreakPoints.isEmpty())
  {
    m_pSeriesBreakPoints->attachAxis(m_pValueAxisY);
    m_pSeriesBreakPoints->attachAxis(m_pValueAxisX);
    m_pSeriesBreakPoints->replace(m_BreakPoints);
  }

  m_pChart->update(m_pUi->graphicsView->rect());
  m_pScene->update(m_pScene->sceneRect());
}

void Plotter::on_xmin_valueChanged(const QString &arg1)
{
  m_pUi->xmax->setMinimum(arg1.toInt());
  m_pValueAxisX->setRange(arg1.toDouble(),m_pUi->xmax->value());
  ReCalculateAndUpdate();
}


void Plotter::on_xmax_valueChanged(const QString &arg1)
{
  m_pUi->xmin->setMaximum(arg1.toInt());
  m_pValueAxisX->setRange(m_pUi->xmin->value(),arg1.toDouble());
  ReCalculateAndUpdate();
}

void Plotter::on_ymin_valueChanged(const QString &arg1)
{
  m_pUi->ymax->setMinimum(arg1.toInt());
  m_pValueAxisY->setRange(arg1.toDouble(),m_pUi->ymax->value());
  ReCalculateAndUpdate();
}


void Plotter::on_ymax_valueChanged(const QString &arg1)
{
  m_pUi->ymin->setMaximum(arg1.toInt());
  m_pValueAxisY->setRange(m_pUi->ymin->value(),arg1.toDouble());
  ReCalculateAndUpdate();
}

void Plotter::SetCursor(QPointF point)
{
    m_pLinesCursor->clear();
    m_pCursor->clear();
    m_pLinesCursor->attachAxis(m_pValueAxisY);
    m_pLinesCursor->attachAxis(m_pValueAxisX);
    m_pLinesCursor->append(point.x(),0); //dont replace
    m_pLinesCursor->append(point);
    m_pLinesCursor->append(0,point.y());//
    m_pCursor->attachAxis(m_pValueAxisY);
    m_pCursor->attachAxis(m_pValueAxisX);
    m_pCursor->append(point);
    m_pChart->update(m_pUi->graphicsView->rect());
    m_pScene->update(m_pScene->sceneRect());
}

void Plotter::on_cur_val_slider_valueChanged(int value)
{
 if(value<m_Result.length() && value>=0)
  {
    m_pUi->value_x_in_point->setText(QString::number(m_Result[value].x(),10,2));
    m_pUi->func->setText(QString::number(m_Result[value].y(),10,m_Prec));
    SetCursor(m_Result[value]);
    m_pLabelCursor->setText(QString("X: %1 \nY: %2 ").arg(m_Result[value].x()).arg(m_Result[value].y()));
    m_pLabelCursor->setAnchor(m_Result[value]);
    m_pLabelCursor->setZValue(11);
    m_pLabelCursor->updateGeometry();
    m_pLabelCursor->show();
  }
}

void Plotter::on_precision_Fx_valueChanged(int value)
{
 switch(value)
 {
  case 1: {m_Prec=1; break;}
  case 2: {m_Prec=2; break;}
  case 3: {m_Prec=3; break;}
  case 4: {m_Prec=4; break;}
  case 5: {m_Prec=5; break;}
  case 6: {m_Prec=6; break;}
 }
}

void Plotter::on_ContextMenuCall(QPoint val)
{
       QMenu * menu = new QMenu(this);

       QAction *options = new QAction("Options", this);
       QAction *hide_numbers = new QAction("Hide numbers", this);
       QAction *hide_names = new QAction("Hide names of axis", this);
       QAction *save_graph = new QAction("Save graph as PNG", this);
       QAction *hide_legend = new QAction("Hide chart legend", this);

       connect(hide_numbers, SIGNAL(triggered()), this, SLOT(on_HideNumbers()));
       connect(hide_names, SIGNAL(triggered()), this, SLOT(on_HideNames()));
       connect(save_graph, SIGNAL(triggered()), this, SLOT(on_SaveGraph()));
       connect(options, SIGNAL(triggered()), this, SLOT(on_Options()));
       connect(hide_legend, SIGNAL(triggered()), this, SLOT(on_HideLegend()));

       menu->addAction(options);
       menu->addAction(hide_numbers);
       menu->addAction(hide_names);
       menu->addAction(hide_legend);
       menu->addAction(save_graph);

       menu->popup(m_pUi->PlotterWidget->mapToGlobal(val));
}

void Plotter::on_HideNumbers()
{
    if(m_NumberAxisIsHidden)
    {
        m_pValueAxisX->setLabelsVisible(false);
        m_pValueAxisY->setLabelsVisible(false);
        m_NumberAxisIsHidden=false;
    }
    else
    {
        m_pValueAxisX->setLabelsVisible(true);
        m_pValueAxisY->setLabelsVisible(true);
        m_NumberAxisIsHidden=true;
    }
}

void Plotter::on_HideNames()
{
    if(m_NamesAxisIsHidden)
    {
        m_pValueAxisX->setTitleText("");
        m_pValueAxisY->setTitleText("");
        m_NamesAxisIsHidden=false;
    }
    else
    {
        m_pValueAxisX->setTitleText("Axis x");
        m_pValueAxisY->setTitleText("Axis y");
        m_NamesAxisIsHidden=true;
    }
    m_pScene->update(m_pScene->sceneRect());
}

void Plotter::on_SaveGraph()
{
    QPixmap* picture=new QPixmap;
    *picture=m_pUi->graphicsView->grab();
    QString filename = QFileDialog::getSaveFileName(this, tr("Save file"), "", tr("Images (*.png)"));
    *picture=picture->copy(0,0,750,560);
    picture->save(filename,"PNG");
    delete picture;
}

void Plotter::on_Options()
{
    m_pPlotterMenu->setFixedSize(m_pPlotterMenu->size());
    m_pPlotterMenu->show();
    m_pMainChart=&(m_pPlotterMenu->ChartToSet);
}

void Plotter::on_HideLegend()
{
    if(m_ChartLegendIsHidden)
    {
        m_pChart->legend()->setVisible(false);
        m_ChartLegendIsHidden=false;
    }
    else
    {
        m_pChart->legend()->setVisible(true);
        m_ChartLegendIsHidden=true;
    }
}

void Plotter::on_SetChartSettings()
{
    if(m_pMainChart!=nullptr && m_pMainChart->isChange)
    {
       m_pSeriesBreakPoints->setColor(m_pMainChart->BackgroundGraph);
       m_pSeriesBreakPoints->setBorderColor(m_pMainChart->BreakPointColor);
       m_pSeriesBreakPoints->setMarkerSize(m_pMainChart->ThinknessBreakPoint);

       m_pChart->setPlotAreaBackgroundBrush(QBrush(m_pMainChart->BackgroundGraph));
       m_pChart->setPlotAreaBackgroundVisible(true);
       m_pChart->setBackgroundBrush(QBrush(m_pMainChart->Background));
       m_pChart->setTitleFont(m_pMainChart->GraphFont);

       m_pSeries0->setPen(m_pMainChart->GraphPen);

       UpdateGraph();
    }
}

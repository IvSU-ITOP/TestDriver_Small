#include "Plotter.h"
#include <OptionMenuPlotter.h>
#include <Parser.h>
#include <ui_Plotter.h>

Plotter::Plotter(QObject *parent)
   : QMainWindow(nullptr)
   ,m_pUi(new Ui::Plotter)
  {
      m_pUi->setupUi(this);

      m_pUi->PlotterWidget->setContextMenuPolicy(Qt::CustomContextMenu);
      m_pChartView = new QChartView(m_pChart);
      m_pScene=new QGraphicsScene(m_pUi->graphicsView);
      m_pUi->value_x_in_point->setText( QString::number( m_pUi->cur_val_slider->value() ) );
      m_pUi->cur_val_slider->setMinimum(0);
      m_pUi->cur_val_slider->setMaximum(10);


      m_pUi->xmin->setMinimum(-100);
      m_pUi->xmax->setMaximum(100);
      m_pUi->ymin->setMinimum(-1000);
      m_pUi->ymax->setMaximum(1000);

      connect(m_pUi->PlotterWidget, SIGNAL(customContextMenuRequested(QPoint)), this, SLOT(on_ContextMenuCall(QPoint)));
      connect(m_pPlotterMenu,SIGNAL(sendDataClass(void)),this,SLOT(on_SetChartSettings(void)));

      QColor color_pen("black");
      m_Pen.setColor(color_pen);

      //m_pSeries0->setUseOpenGL(true);// May make some trouble
  }


Plotter::~Plotter()
  {
    delete m_pUi;
  }

QByteArray Plotter::FindExprScob(int index)
{
    QByteArray ExprScob{};
    int counter_scob{},i{index};
    do
    {
        if(m_Formula[i]=='(') counter_scob++;
        if(m_Formula[i]==')') counter_scob--;
        ExprScob.append(m_Formula[i]);
        i++;
    }
    while (i<m_Formula.length() && counter_scob!=0);
    if(ExprScob[ExprScob.length()-1]==')')ExprScob.remove(ExprScob.length()-1,1);
    if(ExprScob[0]=='(') ExprScob.remove(0,1);
    return ExprScob;
}

void Plotter::DomainFunction(double *pX_start, double *pX_end, double *pX_step)
{
   if(m_Formula.contains("/"))
   {
       QByteArray ExprScob=FindExprScob(m_Formula.indexOf("/")+1);
       bool isX = ExprScob.indexOf('x') != -1;
       bool isY = ExprScob.indexOf('y') != -1;
       for( double X = *pX_start; ceil(X) < *pX_end; X += *pX_step)
       {
         if(fabs(X) < 0.5 * (*pX_step) ) X = 0;
         QByteArray CheckExpr(ExprScob);
         QByteArray TextX(QByteArray::number(X));
         if(X < 0) TextX = '(' + TextX + ')';

         if(isX)  CheckExpr.replace('x', TextX);
         if(isY)  CheckExpr.replace('y', TextX);
         MathExpr Expr = MathExpr( Parser::StrToExpr(CheckExpr));
         if( s_GlobalInvalid || Expr.IsEmpty() )m_BadPoints.append(X);
         Expr=Expr.SimplifyFull();
         Expr->ResetPrecision(m_Precision);
         TConstant *pValue =CastPtr(TConstant, Expr);

         if(pValue == nullptr) break;
         if(pValue->Value()==0)
         {
            m_BreakPointsX.append(X);
            m_BadPoints.append(X);
          }
        }
    }

   if(m_Formula.contains("exp"))
   {
       QByteArray ExprScob=FindExprScob(m_Formula.indexOf("exp")+4);
       ExprScob.append(')');
       bool isX = ExprScob.indexOf('x') != -1;
       bool isY = ExprScob.indexOf('y') != -1;
       for( double X = *pX_start; ceil(X) < *pX_end; X += *pX_step)
       {
         if(fabs(X) < 0.5 * (*pX_step) ) X = 0;
         QByteArray CheckExpr(ExprScob);
         QByteArray TextX(QByteArray::number(X));
         if(X < 0) TextX = '(' + TextX + ')';
         if(isX) CheckExpr.replace('x', TextX);
         if(isY)  CheckExpr.replace('y', TextX);
         CheckExpr.prepend(m_Formula.mid(m_Formula.indexOf("exp"),4));
         MathExpr Expr = MathExpr( Parser::StrToExpr(CheckExpr));
         if( s_GlobalInvalid || Expr.IsEmpty() )m_BadPoints.append(X);
         Expr=Expr.SimplifyFull();
         Expr->ResetPrecision(m_Precision);
         TConstant *pValue =CastPtr(TConstant, Expr);
         if(pValue == nullptr) break;
         if(X<0)m_BadPoints.append(X);
       }
   }
   if(m_Formula.contains("lg"))
   {
       QByteArray ExprScob=FindExprScob(m_Formula.indexOf("lg")+2);
       bool isX = ExprScob.indexOf('x') != -1;
       bool isY = ExprScob.indexOf('y') != -1;
       for( double X = *pX_start; ceil(X) < *pX_end; X += *pX_step)
       {
         if(fabs(X) < 0.5 * (*pX_step) ) X = 0;
         QByteArray CheckExpr(ExprScob);
         QByteArray TextX(QByteArray::number(X));
         if(X < 0) TextX = '(' + TextX + ')';
         if(isX)  CheckExpr.replace('x', TextX);
         if(isY)  CheckExpr.replace('y', TextX);
         MathExpr Expr = MathExpr( Parser::StrToExpr(CheckExpr));
         if( s_GlobalInvalid || Expr.IsEmpty() )m_BadPoints.append(X);
         Expr=Expr.SimplifyFull();
         Expr->ResetPrecision(m_Precision);
         TConstant *pValue =CastPtr(TConstant, Expr);
         if(pValue == nullptr) break;
         if(pValue->Value()<0+(*pX_step))m_BadPoints.append(X);
       }
   }
   if(m_Formula.contains("log"))
   {
       QByteArray ExprScob = FindExprScob(m_Formula.indexOf("log")+3);
      // QByteArray ExprScob=m_Formula.mid(m_Formula.indexOf("log"),end);
       bool isX = ExprScob.indexOf('x') != -1;
       bool isY = ExprScob.indexOf('y') != -1;
       for( double X = *pX_start; ceil(X) < *pX_end; X += *pX_step)
       {
         if(fabs(X) < 0.5 * (*pX_step) ) X = 0;
         QByteArray CheckExpr(ExprScob);
         QByteArray TextX(QByteArray::number(X));
         if(X < 0) TextX = '(' + TextX + ')';
         if(isX)  CheckExpr.replace('x', TextX);
         if(isY)  CheckExpr.replace('y', TextX);
         MathExpr Expr = MathExpr( Parser::StrToExpr(CheckExpr));
         if( s_GlobalInvalid || Expr.IsEmpty() )m_BadPoints.append(X);
         else
         {
         Expr=Expr.SimplifyFull();
         Expr->ResetPrecision(m_Precision);
         TConstant *pValue =CastPtr(TConstant, Expr);
         if(pValue == nullptr) break;
         if(pValue->Value()<0+(*pX_step))m_BadPoints.append(X);
         }
       }
   }
   if(m_Formula.contains("ln"))
   {
       QByteArray ExprScob=FindExprScob(m_Formula.indexOf("ln")+2);
       bool isX = ExprScob.indexOf('x') != -1;
       bool isY = ExprScob.indexOf('y') != -1;
       for( double X = *pX_start; ceil(X) < *pX_end; X += *pX_step)
       {
         if(fabs(X) < 0.5 * (*pX_step) ) X = 0;
         QByteArray CheckExpr(ExprScob);
         QByteArray TextX(QByteArray::number(X));
         if(X < 0) TextX = '(' + TextX + ')';
         if(isX)  CheckExpr.replace('x', TextX);
         if(isY)  CheckExpr.replace('y', TextX);
         MathExpr Expr = MathExpr( Parser::StrToExpr(CheckExpr));
         if( s_GlobalInvalid || Expr.IsEmpty() )m_BadPoints.append(X);
         Expr=Expr.SimplifyFull();
         Expr->ResetPrecision(m_Precision);
         TConstant *pValue =CastPtr(TConstant, Expr);
         if(pValue == nullptr) break;

         if(pValue->Value()<0+(*pX_step))m_BadPoints.append(X);
       }
   }
}

QVector <QPointF> Plotter::CalculatePoint()
  {
    if( m_Formula.isEmpty() || m_Formula.contains("/0") )
      return {};

    m_Formula=m_Formula.toLower();

    if(m_Formula.contains("y")&&m_Formula[m_Formula.indexOf('y')+1]=='=')
    {
        m_Formula.remove(m_Formula.indexOf('y'),2);
    }

    bool isX = m_Formula.indexOf('x') != -1;
    bool isY = m_Formula.indexOf('y') != -1;

    if(!isX && !isY)
      return {};

    QVector <QPointF> Result;

    double Y;
    double X_start(m_pUi->xmin->value()), X_end(m_pUi->xmax->value()), X_step(0.05);

    DomainFunction(&X_start,&X_end,&X_step);
    number_of_breakpoints.clear();
     for( double X = X_start; ceil(X) < X_end; X += X_step)
       {
           if(fabs(X) < 0.5 * X_step )
           {
               X = 0;
            }
           if(!m_BadPoints.isEmpty() && m_BadPoints.contains(X))
           {
              if(m_BreakPointsX.contains(X))number_of_breakpoints.append(Result.length()-1);
           }
           else
           {
               QByteArray Formula_copy(m_Formula);
               QByteArray TextX(QByteArray::number(X));
               if(X < 0) TextX = '(' + TextX + ')';
               Formula_copy.remove(m_Formula.indexOf("exp"),3);
               if(isX) Formula_copy.replace('x', TextX);
               if(isY) Formula_copy.replace('y', TextX);
               Formula_copy.insert(m_Formula.indexOf("exp"),m_Formula.mid(m_Formula.indexOf("exp"),3));
               MathExpr Expr = MathExpr( Parser::StrToExpr( Formula_copy));

               if( s_GlobalInvalid || Expr.IsEmpty() )
                 return {};

                //Expr = Expr.Reduce();
                Expr->ResetPrecision(m_Precision);
                Expr=Expr.SimplifyFull();

                TConstant *pValue =CastPtr(TConstant, Expr);
                  if(!(pValue==nullptr))
                  {
                      Y = pValue->Value();
                      if(Y < m_YStart) m_YStart = Y;
                      if( Y > m_YEnd ) m_YEnd = Y;
                      Result.append(QPointF(X, Y));
                  }
           }
        }

     if(!Result.isEmpty() && !number_of_breakpoints.isEmpty())
     {
         for(int i{};i<number_of_breakpoints.length();i++)
         {
            m_BreakPoints.append( QPoint(m_BadPoints[i],ceil(Result[number_of_breakpoints[i]].y())) );
         }
     }

    return Result;
 }

bool Plotter::Plot(QByteArray Formula)
  {
      m_Formula=Formula;
      m_Result=CalculatePoint();

      if(m_Result.isEmpty())
        {
            this->~Plotter();
            return false;
        }

      m_Pen.setWidth(3);
      if(!m_BreakPoints.isEmpty())
      {
          int j{};
          for(int i{};i<=number_of_breakpoints.length();i++)
          {

              m_Series.append(new QLineSeries);
              if(i==number_of_breakpoints.length())
              {
                  while(j<m_Result.length())
                  {
                      m_Series[i]->append(m_Result[j]);
                      j++;
                  }
              }
              else
              {
                  while(j<=number_of_breakpoints[i])
                  {
                      m_Series[i]->append(m_Result[j]);
                      j++;
                  }
              }
              m_Series[i]->setPen(m_Pen);
              m_Series[i]->attachAxis(m_pAxisY);
              m_Series[i]->attachAxis(m_pAxisX);
              m_pChart->addSeries(m_Series[i]);
          }
      }
      else
      {
          for(int i{};i<m_Result.length();i++)
          {
              m_pSeries0->append(m_Result[i]);
          }
           m_pSeries0->setPen(m_Pen);
           m_pSeries0->attachAxis(m_pAxisY);
           m_pSeries0->attachAxis(m_pAxisX);
           m_pChart->addSeries(m_pSeries0);
      }

        for(int i{};i<m_BreakPoints.length();i++)
        {
        m_pSeriesBreakPoint->append(m_BreakPoints[i]);
        }

        m_NeedReCalc=false;
        m_pUi->xmin->setValue( int(m_Result.first().x()) );
        m_pUi->xmax->setValue( int(m_Result.last().x()) );

        m_pUi->ymin->setValue( floor(m_YStart) );
        m_pUi->ymax->setValue( ceil(m_YEnd) );
        m_NeedReCalc=true;

        m_pSeriesBreakPoint->attachAxis(m_pAxisY);
        m_pSeriesBreakPoint->setColor("red");
        m_pSeriesBreakPoint->setMarkerShape(QScatterSeries::MarkerShapeCircle);
        m_pSeriesBreakPoint->setMarkerSize(8);
        m_pSeriesBreakPoint->setBorderColor("red");
        m_pSeriesBreakPoint->attachAxis(m_pAxisY);
        m_pSeriesBreakPoint->attachAxis(m_pAxisX);

        m_pChart->addSeries(m_pSeriesBreakPoint);

        m_pAxisX->setRange(m_pUi->xmin->value(),m_pUi->xmax->value());
        m_pAxisY->setRange(floor(m_YStart), ceil(m_YEnd) );

        m_pAxisX->setTitleText("Axis x");
        m_pAxisX->setLabelsVisible(true);

        m_pAxisY->setTitleText("Axis y");
        m_pAxisY->setLabelsVisible(true);

        m_pAxisX->setTickCount(20);
        m_pAxisY->setTickCount(20);
        m_pAxisX->applyNiceNumbers();
        m_pAxisY->applyNiceNumbers();

        m_pChart->setTitle(m_Formula);
        m_pChart->addAxis(m_pAxisX,Qt::AlignBottom);
        m_pChart->addAxis(m_pAxisY,Qt::AlignLeft);

        m_pChart->setGeometry( m_pUi->graphicsView->rect());
        m_pChartView->setRenderHint(QPainter::Antialiasing,true);
        m_pScene->addItem(m_pChart);

        m_pUi->graphicsView->setScene(m_pScene);

        ConfigureGraph();
        return true;
}

void Plotter::ReCalculate()
  {
      m_BadPoints.clear();
      m_BreakPoints.clear();

      m_Result=CalculatePoint();
      m_pUi->cur_val_slider->setMaximum(m_Result.length());
 }


void Plotter::ConfigureGraph()
{

    if(!m_BreakPoints.isEmpty())
    {
    QVector <QPointF> TempResult{};
        int j{};
        for(int i{};i<=number_of_breakpoints.length();i++)
        {
             m_Series.append(new QLineSeries);
             if(i==number_of_breakpoints.length())
             {
                 while(j<m_Result.length())
                 {
                     TempResult.append(m_Result[j]);
                     j++;
                 }

             }
             else
             {
                 while(j<=number_of_breakpoints[i])
                 {
                     TempResult.append(m_Result[j]);
                     j++;
                 }
             }
            m_Series[i]->replace(TempResult);
            TempResult.clear();
            m_Series[i]->setPen(m_Pen);
            m_Series[i]->attachAxis(m_pAxisY);
            m_Series[i]->attachAxis(m_pAxisX);
            m_pChart->addSeries(m_Series[i]);
        }

    }
    else
    {
        m_pSeries0->setPen(m_Pen);
        m_pSeries0->attachAxis(m_pAxisY);
        m_pSeries0->attachAxis(m_pAxisX);
        m_pSeries0->replace(m_Result);

    }
    m_pSeriesBreakPoint->attachAxis(m_pAxisY);
    m_pSeriesBreakPoint->attachAxis(m_pAxisX);
    m_pSeriesBreakPoint->replace(m_BreakPoints);
    m_pChart->update(m_pUi->graphicsView->rect());
    m_pScene->update(m_pScene->sceneRect());
}

void Plotter::on_xmin_valueChanged(const QString &arg1)
{
    if(m_NeedReCalc)
    {
        m_pUi->xmax->setMinimum(arg1.toInt());
        m_pAxisX->setRange(arg1.toDouble(),m_pUi->xmax->value());
        ReCalculate();
        ConfigureGraph();
    }
}


void Plotter::on_xmax_valueChanged(const QString &arg1)
{
    if(m_NeedReCalc)
    {
        m_pUi->xmin->setMaximum(arg1.toInt());
        m_pAxisX->setRange(m_pUi->xmin->value(),arg1.toDouble());
        ReCalculate();
        ConfigureGraph();
    }
}

void Plotter::on_ymin_valueChanged(const QString &arg1)
{

    if(m_NeedReCalc)
    {
        m_pUi->ymax->setMinimum(arg1.toInt());
        m_pAxisY->setRange(arg1.toDouble(),m_pUi->ymax->value());
        ReCalculate();
        ConfigureGraph();
    }
}


void Plotter::on_ymax_valueChanged(const QString &arg1)
{
    if(m_NeedReCalc)
    {
        m_pUi->ymin->setMaximum(arg1.toInt());
        m_pAxisY->setRange(m_pUi->ymin->value(),arg1.toDouble());
        ReCalculate();
        ConfigureGraph();
    }
}


void Plotter::on_cur_val_slider_valueChanged(int value)
{
    if(value<m_Result.length() && value>=0)
    {
        m_pUi->value_x_in_point->setText(QString::number(m_Result[value].x(),10,2));
        m_pUi->func->setText(QString::number(m_Result[value].y(),10,m_Prec));
    }
}

void Plotter::on_precision_Fx_valueChanged(int value)
{
    switch(value)
    {
        case 1: {m_Precision=1e-1;m_Prec=1; break;}
        case 2: {m_Precision=1e-2;m_Prec=2; break;}
        case 3: {m_Precision=1e-3;m_Prec=3; break;}
        case 4: {m_Precision=1e-4;m_Prec=4; break;}
        case 5: {m_Precision=1e-5;m_Prec=5; break;}
        case 6: {m_Precision=1e-6;m_Prec=6; break;}
        default :m_Precision=1e-1;
    }
    ReCalculate();
}

void Plotter::on_ContextMenuCall(QPoint val)
{
       QMenu * menu = new QMenu(this);

       QAction *options = new QAction("Options", this);
       QAction *hide_numbers = new QAction("Hide numbers", this);
       QAction *hide_names = new QAction("Hide names", this);
       QAction *save_graph = new QAction("Save graph", this);
       QAction *hide_grid = new QAction("Hide gridline", this);

       connect(hide_numbers, SIGNAL(triggered()), this, SLOT(on_HideNumbers()));
       connect(hide_names, SIGNAL(triggered()), this, SLOT(on_HideNames()));
       connect(save_graph, SIGNAL(triggered()), this, SLOT(on_SaveGraph()));
       connect(options, SIGNAL(triggered()), this, SLOT(on_Options()));
       connect(hide_grid, SIGNAL(triggered()), this, SLOT(on_HideGrid()));

       menu->addAction(options);
       menu->addAction(hide_numbers);
       menu->addAction(hide_names);
       menu->addAction(hide_grid);
       menu->addAction(save_graph);

       menu->popup(m_pUi->PlotterWidget->mapToGlobal(val));
}

void Plotter::on_HideNumbers()
{
    if(m_NumberAxisIsHidden)
    {
        m_pAxisX->setLabelsVisible(false);
        m_pAxisY->setLabelsVisible(false);
        m_NumberAxisIsHidden=false;
    }
    else
    {
        m_pAxisX->setLabelsVisible(true);
        m_pAxisY->setLabelsVisible(true);
        m_NumberAxisIsHidden=true;
    }
}

void Plotter::on_HideNames()
{
    if(m_NamesAxisIsHidden)
    {
        m_pAxisX->setTitleText("");
        m_pAxisY->setTitleText("");
        m_NamesAxisIsHidden=false;
    }
    else
    {
        m_pAxisX->setTitleText("Axis x");
        m_pAxisY->setTitleText("Axis y");
        m_NamesAxisIsHidden=true;
    }
    m_pScene->update(m_pScene->sceneRect());
}

void Plotter::on_SaveGraph()
{
    QPixmap* picture=new QPixmap;
    *picture=m_pUi->graphicsView->grab();

    /*
    //if using opengl
    QOpenGLWidget *glWidget  = m_pChartView->findChild<QOpenGLWidget*>();
    if(glWidget){
        QPainter painter(picture);
        QPoint d = glWidget->mapToGlobal(QPoint())-m_pChartView->mapToGlobal(QPoint());
        painter.setCompositionMode(QPainter::CompositionMode_SourceAtop);
        painter.drawImage(d, glWidget->grabFramebuffer());
        painter.end();
    }*/

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

void Plotter::on_HideGrid()
{
    if(m_GridAxisIsHidden)
    {
        m_pAxisX->setGridLineVisible(false);
        m_pAxisY->setGridLineVisible(false);
        m_GridAxisIsHidden=false;
    }
    else
    {
        m_pAxisX->setGridLineVisible(true);
        m_pAxisY->setGridLineVisible(true);
        m_GridAxisIsHidden=true;
    }
    m_pScene->update(m_pScene->sceneRect());
}

void Plotter::on_SetChartSettings()
{
    if(m_pMainChart!=nullptr && m_pMainChart->isChange)
    {
        m_Pen.setWidth(m_pMainChart->Thinkness);
        m_Pen.setColor(m_pMainChart->GraphColor);

        m_pSeriesBreakPoint->setColor(m_pMainChart->Cursor);
        m_pSeriesBreakPoint->setBorderColor(m_pMainChart->Cursor);

        m_pChart->setPlotAreaBackgroundBrush(QBrush(m_pMainChart->BackgroundGraph));
        m_pChart->setPlotAreaBackgroundVisible(true);
        m_pChart->setBackgroundBrush(QBrush(m_pMainChart->Background));
        m_pChart->setTitleFont(m_pMainChart->GraphFont);

        m_pAxisX->setTitleFont(m_pMainChart->GraphFont);
        m_pAxisY->setTitleFont(m_pMainChart->GraphFont);
        m_pAxisX->setGridLineColor(m_pMainChart->GridLine);
        m_pAxisY->setGridLineColor(m_pMainChart->GridLine);
        ConfigureGraph();
    }
}



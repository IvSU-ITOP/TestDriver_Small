#include "Plotter.h"

#include <OptionMenuPlotter.h>
#include <Parser.h>
#include <ui_Plotter.h>

Plotter::Plotter(QObject *parent)
   : QMainWindow(nullptr)
  {
      m_pUi=new Ui::Plotter;
      m_pUi->setupUi(this);
      m_pScene=new QGraphicsScene(m_pUi->graphicsView);

      m_pValueAxisX->setTitleText("X");
      m_pValueAxisY->setTitleText("Y");
      m_pValueAxisX->setTickCount(20);
      m_pValueAxisY->setTickCount(20);
      m_pValueAxisX->applyNiceNumbers();
      m_pValueAxisY->applyNiceNumbers();

      m_pSeries0->setPen(m_MainChart.GraphPen);
      m_pSeries0->setUseOpenGL(true);

      m_MainChart.clear();
      m_pSeriesBreakPoints->setBorderColor(m_MainChart.BreakPointColor);
      m_pSeriesBreakPoints->setBorderColor("white");
      m_pSeriesBreakPoints->setMarkerSize(m_MainChart.ThinknessBreakPoint);
      m_pChart->setGeometry( m_pUi->graphicsView->rect());
      m_pChart->addAxis(m_pValueAxisX,Qt::AlignBottom);
      m_pChart->addAxis(m_pValueAxisY,Qt::AlignLeft);
  }


Plotter::~Plotter()
  {
    delete m_pUi;
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

    if(!m_Formula.contains('x') && !m_Formula.contains('y')) return {};

    QVector <QPointF> Result;

    double Y;
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

               if( s_GlobalInvalid || Expr.IsEmpty() )
                 return {};

                Expr=Expr.SimplifyFull();

               if(s_GlobalInvalid && s_LastError=="INFVAL" )
                {
                   m_BreakPoints.append(QPointF(X, Y));
                }

                TConstant *pValue =CastPtr(TConstant, Expr);
                  if(!(pValue==nullptr) && !s_GlobalInvalid)
                  {
                      Y = pValue->Value();
                      if(Y < m_YMin) m_YMin = Y;
                      if( Y > m_YMax ) m_YMax = Y;
                      Result.append(QPointF(X, Y));
                  }

        }
    return Result;
 }

bool Plotter::Plot(QByteArray Formula)
  {
       m_Formula=Formula;
       m_Result=CalculatePoint();

       m_pValueAxisX->setRange(m_pUi->xmin->value(),m_pUi->xmax->value());
       m_pValueAxisY->setRange(floor(m_YMin), ceil(m_YMax) );

       if(m_Result.isEmpty())
       {
            this->~Plotter();
            return false;
       }

        m_pSeries0->replace(m_Result);
        m_pSeriesBreakPoints->replace(m_BreakPoints);
        m_pSeries0->attachAxis(m_pValueAxisY);
        m_pSeries0->attachAxis(m_pValueAxisX);
        m_pSeriesBreakPoints->attachAxis(m_pValueAxisY);
        m_pSeriesBreakPoints->attachAxis(m_pValueAxisX);
        m_pChart->addSeries(m_pSeries0);
        m_pChart->addSeries(m_pSeriesBreakPoints);
        m_pChart->setTitle(m_Formula);

        m_pScene->addItem(m_pChart);
        m_pUi->graphicsView->setScene(m_pScene);
        return true;
}


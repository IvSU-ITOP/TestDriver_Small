#include "Plotter.h"
#include <Parser.h>
#include <ui_Plotter.h>

Plotter::Plotter(QObject *parent)
   : QMainWindow(nullptr)
   ,m_pUi(new Ui::Plotter)
  {
  m_pUi->setupUi(this);
  m_pChartView = new QChartView(m_pChart);
  m_pScene=new QGraphicsScene(m_pUi->graphicsView);
  m_pUi->value_x_in_point->setText( QString::number( m_pUi->cur_val_slider->value() ) );
  m_pUi->cur_val_slider->setMinimum(0);
  m_pUi->cur_val_slider->setMaximum(10);


  m_pUi->xmin->setMinimum(-10000);
  m_pUi->xmax->setMaximum(10000);
  m_pUi->ymin->setMinimum(-10000);
  m_pUi->ymax->setMaximum(10000);


  QColor color_pen("black");
  m_Pen.setColor(color_pen);

  connect(this, SIGNAL(value_x_in_point_changed(int)), m_pUi->value_x_in_point, SLOT(on_cur_value_changed(int)) );
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

    bool isX = m_Formula.indexOf('x') != -1;
    bool isY = m_Formula.indexOf('y') != -1;

    if(!isX && !isY)
      return {};

    QVector <QPointF> Result;

    double Y;
    double X_start(m_pUi->xmin->value()), X_end(m_pUi->xmax->value()), X_step(0.05);
    if(m_Formula.contains("~"))
      {
      QByteArray Number;
      for(int Number_counter=m_Formula.indexOf('~')+2; m_Formula[Number_counter] != ')'; Number_counter++)
        Number += m_Formula[Number_counter];
      if(Number.toInt()%2==0) X_start=0;
      }

    if(m_Formula.contains("lg")||m_Formula.contains("log")||m_Formula.contains("ln"))
      X_start = X_step;

    /*
      Vals=TExpressionValues.Create(XAxe.Min,XAxe.Max,XAxe.Step/20);
      if Assigned(GraphLine) and Assigned(GraphLine.CalcExpression) then begin
       DifStr:= GraphLine.Expression;
       TExpr DrvExpr= TExpression::Parse(m_Formula);
       DifStr=  Expr.GetDifMathString();
       DrvExpr:= TExpression.Parse(DifStr);
       DifStr:=  DrvExpr.GetMathString;
    */

     for( double X = X_start; ceil(X) < X_end; X += X_step)
       {
       if(fabs(X) < 0.5 * X_step ) X = 0;
       QByteArray Formula_copy(m_Formula);
       QByteArray TextX(QByteArray::number(X));
       if(X < 0) TextX = '(' + TextX + ')';

       if(isX) Formula_copy.replace('x', TextX);
       if(isY) Formula_copy.replace('y', TextX);

       MathExpr Expr = MathExpr( Parser::StrToExpr( Formula_copy));

       if( s_GlobalInvalid || Expr.IsEmpty() )
         return {};

       //Expr = Expr.Reduce();
        Expr=Expr.SimplifyFull();
        Expr->ResetPrecision(m_Precision);
        TConstant *pValue =CastPtr(TConstant, Expr);
        if(pValue == nullptr) break;
/*
       bool test_complex=Expr.Ptr()->m_Contents.contains(0xe6);
       bool test_false=Expr.Ptr()->m_Contents.contains("FALSE");
       bool test_power=Expr.Ptr()->m_Contents.contains('^');
       bool test_inf=Expr.Ptr()->m_Contents.contains("-1E11");
       if(!test_complex && !test_false && !test_power && !test_inf)
       {
*/
       Y = pValue->Value();
       if(Y < m_YStart) m_YStart = Y;
       if( Y > m_YEnd ) m_YEnd = Y;
       Result.append(QPointF(X, Y));
//       }

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

  for(int i = 0; i < m_Result.length(); i++)
    {
    m_pSeries0->append(m_Result[i]);
    }

    m_NeedReCalc=false;
    m_pUi->xmin->setValue( int(m_Result.first().x()) );
    m_pUi->xmax->setValue( int(m_Result.last().x()) );
    m_pUi->ymin->setValue( int(m_YStart) );
    m_pUi->ymax->setValue( int(m_YEnd) );
    m_NeedReCalc=true;

    m_pAxisX->setRange(m_pUi->xmin->value(),m_pUi->xmax->value());
    m_pAxisX->setTitleText("Axis x");
    m_pAxisX->setLabelsVisible(true);

    m_pAxisY->setRange(m_YStart, m_YEnd);
    m_pAxisY->setTitleText("Axis y");
    m_pAxisY->setLabelsVisible(true);

    m_pChart->setTitle("Your graph");
    ConfigureGraph();
    return true;
}


void Plotter::ReCalculate()
  {
  delete m_pSeries0;
  m_pSeries0 = new QLineSeries;
  m_Result=CalculatePoint();
  for(int i=0; i < m_Result.length(); i++)
    {
    if( m_Result[i].y() > m_pUi->ymin->value() && m_Result[i].y() < m_pUi->ymax->value() )
     m_pSeries0->append(m_Result[i]);
   }
  m_pSeries0->attachAxis(m_pAxisY);
  m_pUi->cur_val_slider->setMaximum(m_Result.length());
  }

void Plotter::ConfigureGraph()
{
    m_Pen.setWidth(3);
    m_pSeries0->setPen(m_Pen);

    m_pAxisX->setTickCount(20);
    m_pAxisY->setTickCount(20);

    m_pChart->addSeries(m_pSeries0);
    m_pAxisX->applyNiceNumbers();
    m_pAxisY->applyNiceNumbers();

    m_pChart->setAxisX(m_pAxisX,m_pSeries0);
    m_pChart->setAxisY(m_pAxisY,m_pSeries0);

    m_pChart->addAxis(m_pAxisX,Qt::AlignBottom);
    m_pChart->addAxis(m_pAxisY,Qt::AlignLeft);

    m_pChart->setGeometry( m_pUi->graphicsView->rect());
    m_pChartView->setRenderHint(QPainter::Antialiasing,true);
    m_pScene->addItem(m_pChart);
    m_pUi->graphicsView->setScene(m_pScene);
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


void Plotter::on_precision_x_valueChanged(int value) //FIXME //precision Fx rename
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
    ConfigureGraph();
}


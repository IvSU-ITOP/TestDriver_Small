#ifndef PLOTTER_H
#define PLOTTER_H

#include <QMainWindow>
#include <iostream>
#include <QtCharts/QChartView>
#include <QtCharts/QLineSeries>
#include <QtCharts/QAreaSeries>
#include <QtCharts/QValueAxis>

using namespace QtCharts;

QT_BEGIN_NAMESPACE
namespace Ui { class Plotter; }
QT_END_NAMESPACE

class Plotter : public QMainWindow
  {
  Q_OBJECT
  private:
    Ui::Plotter *m_pUi;
    QLineSeries *m_pSeries0=new QLineSeries;
    QValueAxis *m_pAxisX = new QValueAxis;
    QValueAxis *m_pAxisY = new QValueAxis;
    QGraphicsScene *m_pScene = nullptr;
    QChart *m_pChart = new QChart;
    QChartView *m_pChartView=nullptr;
    QVector<QPointF> m_Result;
    QByteArray m_Formula;
    QPen m_Pen;
    double m_YStart = 1.79769e+308, m_YEnd = 2.22507e-308;
    double m_Precision = 1e-10;
    int m_Prec = 1;
    bool m_NeedReCalc = true;

    void ConfigureGraph();
    void ReCalculate();
    QVector <QPointF> CalculatePoint();

  public:
     Plotter(QObject* parent=nullptr);
     ~Plotter();
     bool Plot(QByteArray Formula);

  private slots:
    void on_xmin_valueChanged(const QString &arg1);
    void on_xmax_valueChanged(const QString &arg1);
    void on_ymin_valueChanged(const QString &arg1);
    void on_ymax_valueChanged(const QString &arg1);
    void on_cur_val_slider_valueChanged(int value);
    void on_precision_x_valueChanged(int value);

  };

#endif // PLOTTER_H

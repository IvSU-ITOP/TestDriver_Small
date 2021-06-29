#ifndef PLOTTER_H
#define PLOTTER_H

#include "OptionMenuPlotter.h"
#include <QAction>
#include <QMainWindow>
#include <QtCharts/QtCharts>
#include <QtCharts/QLineSeries>
#include <QtCharts/QScatterSeries>
#include <QtCharts/QAreaSeries>
#include <QtCharts/QValueAxis>
#include "callout.h"

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
    QScatterSeries *m_pSeriesBreakPoints=new QScatterSeries;

    QValueAxis *m_pValueAxisX = new QValueAxis;
    QValueAxis *m_pValueAxisY = new QValueAxis;
    QGraphicsScene *m_pScene = nullptr;
    QChart *m_pChart = new QChart;
    QVector<QPointF> m_Result{};
    QVector<QPointF> m_BreakPoints{};
    QByteArray m_Formula;
    SettingsChart m_MainChart=SettingsChart();
    double m_YMin = 1.79769e+308, m_YMax = 2.22507e-308;
    int m_Prec = 1;

    void ConfigureGraph();
    QVector <QPointF> CalculatePoint();


public:
     Plotter(QObject* parent=nullptr);
     ~Plotter();
     bool Plot(QByteArray Formula);

};

#endif // PLOTTER_H

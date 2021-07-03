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
    QLineSeries *m_pLinesCursor=new QLineSeries;
    QScatterSeries *m_pSeriesBreakPoints=new QScatterSeries;
    QScatterSeries *m_pCursor=new QScatterSeries;
    QValueAxis *m_pValueAxisX = new QValueAxis;
    QValueAxis *m_pValueAxisY = new QValueAxis;
    QGraphicsScene *m_pScene = nullptr;
    QGraphicsPathItem* m_pPathItem = nullptr;
    QChart *m_pChart = new QChart;
    QChartView *chartView = new QChartView(m_pChart);
    QPainterPath m_Path;
    QVector<QPointF> m_Result{};
    QVector<QPointF> m_BreakPoints{};
    QByteArray m_Formula{};
    SettingsChart *m_pMainChart=new SettingsChart;
    OptionMenuPlotter *m_pPlotterMenu=new OptionMenuPlotter(10,nullptr);
    Callout *m_pLabelCursor = new Callout(m_pChart);
    double m_YMin = 1.79769e+308, m_YMax = 2.22507e-308;
    int m_Prec = 1;
    bool m_NumberAxisIsHidden=true;
    bool m_NamesAxisIsHidden=true;
    bool m_GridAxisIsHidden=true;
    bool m_ChartLegendIsHidden=true;

    void ReCalculateAndUpdate();
    void UpdateGraph();
    void SetCursor(QPointF point);
    QVector <QPointF> CalculatePoint();
    void PaintAxis();

public:
     Plotter(QObject* parent=nullptr);
     ~Plotter();
     bool Plot(QByteArray Formula);

private slots:
     void on_precision_Fx_valueChanged(int value);
     void on_cur_val_slider_valueChanged(int value);
     void on_ymax_valueChanged(const QString &arg1);
     void on_ymin_valueChanged(const QString &arg1);
     void on_xmax_valueChanged(const QString &arg1);
     void on_xmin_valueChanged(const QString &arg1);
     void on_ContextMenuCall(QPoint val);
     void on_HideNumbers();
     void on_HideNames();
     void on_SaveGraph();
     void on_Options();
     void on_HideLegend();

public slots:
     void on_SetChartSettings();
};

#endif // PLOTTER_H

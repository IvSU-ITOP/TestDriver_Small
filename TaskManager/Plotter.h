#ifndef PLOTTER_H
#define PLOTTER_H

#include "OptionMenuPlotter.h"
#include <QAction>
#include <QMainWindow>
#include <QtCharts/QChartView>
#include <QtCharts/QLineSeries>
#include <QtCharts/QSplineSeries>
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
    QVector <QLineSeries *> m_Series={new QLineSeries};
    QLineSeries *m_pAxisX=new QLineSeries;
    QLineSeries *m_pAxisY=new QLineSeries;
    QScatterSeries *m_pSeriesBreakPoint = new QScatterSeries;
    QScatterSeries *m_pSeriesCursor = new QScatterSeries;
    QScatterSeries *m_pSeriesLabelPoints = new QScatterSeries;
    QValueAxis *m_pValueAxisX = new QValueAxis;
    QValueAxis *m_pValueAxisY = new QValueAxis;
    QGraphicsScene *m_pScene = nullptr;
    QChart *m_pChart = new QChart;
    Callout *m_pLabelCursor = new Callout(m_pChart);
    OptionMenuPlotter *m_pPlotterMenu=new OptionMenuPlotter(10,nullptr);
    SettingsChart* m_pMainChart=nullptr;

    QAction *m_pOptions = new QAction("Options", this);
    QAction *m_pHideNumbers = new QAction("Hide numbers", this);
    QAction *m_pHideNames = new QAction("Hide names of axis", this);
    QAction *m_pSaveGraph = new QAction("Save graph as PNG", this);
    QAction *m_pHideLegend = new QAction("Hide chart legend", this);

    QVector<QPointF> m_Result{};
    QVector <double> m_BadPoints{};
    QVector <QPointF> m_BreakPoints{};
    QVector <double> m_BreakPointsX{};
    QVector <int> number_of_breakpoints{};

    QByteArray m_Formula;
    QPen m_Pen;
    double m_YStart = 1.79769e+308, m_YEnd = 2.22507e-308;
    double m_Precision = 1e-10;
    int m_Prec = 1;
    bool m_NeedReCalc = true;
    bool m_NumberAxisIsHidden=true;
    bool m_NamesAxisIsHidden=true;
    bool m_GridAxisIsHidden=true;
    bool m_ChartLegendIsHidden=true;

    void ConfigureGraph();
    void ReCalculate();
    void SetCursor(QPointF point);
    QByteArray FindExprScob(int index);
    int FindIndexLastExprScob(int index);
    void DomainFunction(QByteArray ExprToCheck,double pX_start,double pX_end, double pX_step);
    QVector <QPointF> CalculatePoint();
    double CalculateExprScob(QByteArray ExprScob,double X);

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
    void on_precision_Fx_valueChanged(int value);
    void on_ContextMenuCall(QPoint val);
    void on_HideNumbers();
    void on_HideNames();
    void on_SaveGraph();
    void on_Options();
    void on_HideGrid();
    void on_HideLegend();

public slots:
    void on_SetChartSettings();
};

#endif // PLOTTER_H

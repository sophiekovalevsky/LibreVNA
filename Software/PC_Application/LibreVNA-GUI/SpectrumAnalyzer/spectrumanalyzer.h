#ifndef SPECTRUMANALYZER_H
#define SPECTRUMANALYZER_H

#include "appwindow.h"
#include "mode.h"
#include "CustomWidgets/tilewidget.h"
#include "scpi.h"
#include "Traces/tracewidget.h"
#include "Device/virtualdevice.h"

#include <QObject>
#include <QWidget>
#include <QComboBox>
#include <QCheckBox>
#include <QScrollArea>

class SpectrumAnalyzer : public Mode
{
    Q_OBJECT
public:
    SpectrumAnalyzer(AppWindow *window, QString name = "Spectrum Analyzer");

    void deactivate() override;
    void initializeDevice() override;
    void deviceDisconnected() override;

    virtual Type getType() override { return Type::SA;}

    // Only save/load user changeable stuff, no need to save the widgets/mode name etc.
    virtual nlohmann::json toJSON() override;
    virtual void fromJSON(nlohmann::json j) override;

    void updateGraphColors();
    void setAveragingMode(Averaging::Mode mode) override;

    void preset() override;

    virtual void deviceInfoUpdated() override;

private:
    static QString WindowToString(VirtualDevice::SASettings::Window w);
    static VirtualDevice::SASettings::Window WindowFromString(QString s);
    static QString DetectorToString(VirtualDevice::SASettings::Detector d);
    static VirtualDevice::SASettings::Detector DetectorFromString(QString s);

private slots:
    void NewDatapoint(VirtualDevice::SAMeasurement m);
    // Sweep control
    void SetStartFreq(double freq);
    void SetStopFreq(double freq);
    void SetCenterFreq(double freq);
    void SetSpan(double span);
    void SetFullSpan();
    void SetZeroSpan();
    void SpanZoomIn();
    void SpanZoomOut();
    void SetSingleSweep(bool single);
    // Acquisition control
    void SetRBW(double bandwidth);
    void SetWindow(VirtualDevice::SASettings::Window w);
    void SetDetector(VirtualDevice::SASettings::Detector d);
    void SetAveraging(unsigned int averages);
    void SetSignalID(bool enabled);
    // TG control
    void SetTGEnabled(bool enabled);
    void SetTGPort(int port);
    void SetTGLevel(double level);
    void SetTGOffset(double offset);
    void MeasureNormalization();
    void AbortNormalization();
    void EnableNormalization(bool enabled);
    void ClearNormalization();
    void SetNormalizationLevel(double level);

    void Run();
    void Stop();
    void ConfigureDevice();
    void ResetLiveTraces();

private:
    void SetupSCPI();
    void UpdateAverageCount();
    void SettingsChanged();
    void ConstrainAndUpdateFrequencies();
    void LoadSweepSettings();
    void StoreSweepSettings();

    void createDefaultTracesAndGraphs(int ports);

    VirtualDevice::SASettings settings;
    bool changingSettings;
    unsigned int averages;
    bool singleSweep;
    bool running;
    QTimer configurationTimer;

    double firstPointTime; // timestamp of the first point in the sweep, only use when zerospan is used
    TraceModel traceModel;
    TraceWidget *traceWidget;
    MarkerModel *markerModel;
    Averaging average;

    QScrollArea *central;
    TileWidget *tiles;
    QCheckBox *cbSignalID;
    QComboBox *cbWindowType, *cbDetector;
    QComboBox *cbTrackGenPort;
    QLabel *lAverages;

    struct {
        bool active;
        bool measuring;
        // settings when normalize was measured
        double f_start, f_stop, points;
        // correction values to get the ports to 0dBm
        std::map<QString, std::vector<double>> portCorrection;
        // level to normalize to (additional correction factor)
        SIUnitEdit *Level;

        // GUI elements
        QProgressDialog dialog;
        QPushButton *measure;
        QCheckBox *enable;
    } normalize;

signals:
    void dataChanged();
    void startFreqChanged(double freq);
    void stopFreqChanged(double freq);
    void centerFreqChanged(double freq);
    void spanChanged(double span);
    void singleSweepChanged(bool single);
    void RBWChanged(double RBW);
    void TGStateChanged(bool enabled);
    void TGPortChanged(int port);
    void TGOffsetChanged(double offset);
    void TGLevelChanged(double level);
    void NormalizationLevelChanged(double level);

    void averagingChanged(unsigned int averages);
    void sweepStopped();
    void sweepStarted();
};

#endif // VNA_H

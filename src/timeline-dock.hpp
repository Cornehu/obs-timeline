#pragma once

#include <QWidget>
#include <QVBoxLayout>
#include <QScrollArea>
#include <QTimer>
#include <vector>
#include "source-monitor.hpp"

class SourceTimelineWidget;

class TimelineDock : public QWidget {
	Q_OBJECT

public:
	explicit TimelineDock(QWidget *parent = nullptr);
	~TimelineDock() override = default;

	void refreshSources();
	void stopMonitoring();

private slots:
	void onTimerTick();

private:
	void rebuildWidgets(const std::vector<SourceTimelineInfo> &infos);
	void clearWidgets();

	QVBoxLayout              *m_mainLayout  = nullptr;
	QScrollArea              *m_scrollArea  = nullptr;
	QWidget                  *m_scrollWidget = nullptr;
	QVBoxLayout              *m_scrollLayout = nullptr;
	QTimer                   *m_timer       = nullptr;

	std::vector<SourceTimelineWidget *> m_sourceWidgets;
};

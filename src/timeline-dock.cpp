#include "timeline-dock.hpp"
#include "source-monitor.hpp"

#include <QLabel>
#include <QProgressBar>
#include <QHBoxLayout>
#include <QFrame>
#include <QString>

// ──────────────────────────────────────────────────────────
// SourceTimelineWidget — one row per tracked source
// ──────────────────────────────────────────────────────────
class SourceTimelineWidget : public QFrame {
	Q_OBJECT

public:
	explicit SourceTimelineWidget(const SourceTimelineInfo &info,
				      QWidget *parent = nullptr)
		: QFrame(parent)
	{
		setFrameShape(QFrame::StyledPanel);

		auto *layout = new QVBoxLayout(this);
		layout->setContentsMargins(8, 6, 8, 6);
		layout->setSpacing(4);

		// Source name
		m_nameLabel = new QLabel(QString::fromStdString(info.name), this);
		m_nameLabel->setStyleSheet("font-weight: bold;");
		layout->addWidget(m_nameLabel);

		// Progress bar
		m_progress = new QProgressBar(this);
		m_progress->setMinimum(0);
		m_progress->setMaximum(1000); // per-mille for smooth updates
		m_progress->setTextVisible(false);
		m_progress->setFixedHeight(10);
		layout->addWidget(m_progress);

		// Time row: elapsed — remaining
		auto *timeRow  = new QHBoxLayout();
		m_elapsedLabel = new QLabel(this);
		m_remainLabel  = new QLabel(this);
		m_elapsedLabel->setStyleSheet("color: #aaaaaa; font-size: 11px;");
		m_remainLabel->setStyleSheet("color: #aaaaaa; font-size: 11px;");
		timeRow->addWidget(m_elapsedLabel);
		timeRow->addStretch();
		timeRow->addWidget(m_remainLabel);
		layout->addLayout(timeRow);

		update(info);
	}

	void update(const SourceTimelineInfo &info)
	{
		m_nameLabel->setText(QString::fromStdString(info.name));

		if (info.total > 0) {
			int permille = static_cast<int>(
				(info.elapsed * 1000LL) / info.total);
			m_progress->setValue(qBound(0, permille, 1000));

			int64_t remaining = info.total - info.elapsed;
			m_elapsedLabel->setText(formatMs(info.elapsed));
			m_remainLabel->setText("-" + formatMs(remaining));
		} else {
			m_progress->setValue(0);
			m_elapsedLabel->setText("--:--");
			m_remainLabel->setText("--:--");
		}
	}

	const std::string &sourceName() const { return m_name; }

private:
	static QString formatMs(int64_t ms)
	{
		if (ms < 0) ms = 0;
		int64_t totalSec = ms / 1000;
		int     hours    = static_cast<int>(totalSec / 3600);
		int     minutes  = static_cast<int>((totalSec % 3600) / 60);
		int     seconds  = static_cast<int>(totalSec % 60);

		if (hours > 0)
			return QString::asprintf("%d:%02d:%02d", hours, minutes, seconds);
		return QString::asprintf("%02d:%02d", minutes, seconds);
	}

	std::string   m_name;
	QLabel       *m_nameLabel    = nullptr;
	QProgressBar *m_progress     = nullptr;
	QLabel       *m_elapsedLabel = nullptr;
	QLabel       *m_remainLabel  = nullptr;
};

#include "timeline-dock.moc"

// ──────────────────────────────────────────────────────────
// TimelineDock
// ──────────────────────────────────────────────────────────
TimelineDock::TimelineDock(QWidget *parent) : QWidget(parent)
{
	setMinimumWidth(220);

	m_mainLayout = new QVBoxLayout(this);
	m_mainLayout->setContentsMargins(0, 0, 0, 0);
	m_mainLayout->setSpacing(0);

	// Scrollable area for source rows
	m_scrollArea   = new QScrollArea(this);
	m_scrollWidget = new QWidget(m_scrollArea);
	m_scrollLayout = new QVBoxLayout(m_scrollWidget);
	m_scrollLayout->setContentsMargins(4, 4, 4, 4);
	m_scrollLayout->setSpacing(6);
	m_scrollLayout->addStretch();

	m_scrollWidget->setLayout(m_scrollLayout);
	m_scrollArea->setWidget(m_scrollWidget);
	m_scrollArea->setWidgetResizable(true);
	m_scrollArea->setFrameShape(QFrame::NoFrame);

	m_mainLayout->addWidget(m_scrollArea);

	// Refresh timer — every 250 ms
	m_timer = new QTimer(this);
	m_timer->setInterval(250);
	connect(m_timer, &QTimer::timeout, this, &TimelineDock::onTimerTick);
	m_timer->start();
}

void TimelineDock::onTimerTick()
{
	auto infos = SourceMonitor::getActiveTimelines();
	rebuildWidgets(infos);
}

void TimelineDock::refreshSources()
{
	onTimerTick();
}

void TimelineDock::stopMonitoring()
{
	if (m_timer)
		m_timer->stop();
}

void TimelineDock::rebuildWidgets(const std::vector<SourceTimelineInfo> &infos)
{
	// Fast path: update existing widgets if source list didn't change
	if (infos.size() == m_sourceWidgets.size()) {
		bool same = true;
		for (size_t i = 0; i < infos.size(); ++i) {
			if (infos[i].name != m_sourceWidgets[i]->sourceName()) {
				same = false;
				break;
			}
		}
		if (same) {
			for (size_t i = 0; i < infos.size(); ++i)
				m_sourceWidgets[i]->update(infos[i]);
			return;
		}
	}

	// Source list changed — rebuild
	clearWidgets();
	for (const auto &info : infos) {
		auto *w = new SourceTimelineWidget(info, m_scrollWidget);
		m_scrollLayout->insertWidget(m_scrollLayout->count() - 1, w);
		m_sourceWidgets.push_back(w);
	}
}

void TimelineDock::clearWidgets()
{
	for (auto *w : m_sourceWidgets) {
		m_scrollLayout->removeWidget(w);
		w->deleteLater();
	}
	m_sourceWidgets.clear();
}

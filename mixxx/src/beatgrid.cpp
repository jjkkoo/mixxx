#include <QMutexLocker>
#include <QDebug>

#include "beatgrid.h"

BeatGrid::BeatGrid(TrackPointer pTrack, QByteArray* pByteArray)
        : QObject(),
          m_mutex(QMutex::Recursive),
          m_iSampleRate(pTrack->getSampleRate()),
          m_dBpm(0.0f),
          m_dFirstBeat(0.0f),
          m_dBeatLength(0.0f) {
    connect(pTrack.data(), SIGNAL(bpmUpdated(double)),
            this, SLOT(slotTrackBpmUpdated(double)));

    qDebug() << "New BeatGrid";
    if (pByteArray != NULL) {
        readByteArray(pByteArray);
    }
}

BeatGrid::~BeatGrid() {

}

void BeatGrid::setGrid(double dBpm, double dFirstBeatSample) {
    QMutexLocker lock(&m_mutex);
    m_dBpm = dBpm;
    m_dFirstBeat = dFirstBeatSample;
    m_dBeatLength = 60.0 * m_iSampleRate / m_dBpm;
}

QByteArray* BeatGrid::toByteArray() const {
    QMutexLocker locker(&m_mutex);
    double* pBuffer = new double[2];
    pBuffer[0] = m_dBpm;
    pBuffer[1] = m_dFirstBeat;
    QByteArray* pByteArray = new QByteArray((char*)pBuffer, sizeof(double) * 2);
    delete [] pBuffer;
    return pByteArray;
}

void BeatGrid::readByteArray(QByteArray* pByteArray) {
    if (pByteArray->size() != sizeof(double) * 2) {
        return;
    }
    double* pBuffer = (double*)pByteArray->data();
    setGrid(pBuffer[0], pBuffer[1]);
}

QString BeatGrid::getVersion() const {
    QMutexLocker locker(&m_mutex);
    return "BeatGrid-1.0";
}

// internal use only
bool BeatGrid::isValid() const {
    return m_iSampleRate > 0 && m_dBpm > 0;
}

// This could be implemented in the Beats Class itself.
// If necessary, the child class can redefine it.
double BeatGrid::findNextBeat(double dSamples) const {
    return findNthBeat(dSamples, +1);
}

// This could be implemented in the Beats Class itself.
// If necessary, the child class can redefine it.
double BeatGrid::findPrevBeat(double dSamples) const {
    return findNthBeat(dSamples, -1);
}

// This is an internal call. This could be implemented in the Beats Class itself.
double BeatGrid::findClosestBeat(double dSamples) const {
    QMutexLocker locker(&m_mutex);
    if (!isValid()) {
        return -1;
    }
    double nextBeat = findNextBeat(dSamples);
    double prevBeat = findPrevBeat(dSamples);
    return (nextBeat - dSamples > dSamples - prevBeat) ? prevBeat : nextBeat;
}

double BeatGrid::findNthBeat(double dSamples, int n) const {
    QMutexLocker locker(&m_mutex);
    if (!isValid()) {
        return -1;
    }

    if (n > 0) {
        return (ceilf(dSamples/m_dBeatLength) * m_dBeatLength + m_dFirstBeat) +
                (m_dBeatLength * (n-1));
    }
    else if (n < 0) {
        return (floorf(dSamples/m_dBeatLength) * m_dBeatLength + m_dFirstBeat) -
                (m_dBeatLength * (n+1));
    }

    return -1;
}

void BeatGrid::findBeats(double startSample, double stopSample, QList<double>* pBeatsList) const {
    QMutexLocker locker(&m_mutex);
    if (!isValid()) {
        return;
    }
    if ( startSample > stopSample ) {
        return;
    }
    double curBeat = findNextBeat(startSample);
    while (curBeat <= stopSample) {
        pBeatsList->append(curBeat);
        curBeat += m_dBeatLength;
    }
}

bool BeatGrid::hasBeatInRange(double startSample, double stopSample) const {
    QMutexLocker locker(&m_mutex);
    if (!isValid()) {
        return false;
    }
    double curBeat = findNextBeat(startSample);
    if (curBeat <= stopSample) {
        return true;
    }
    return false;
}

double BeatGrid::getBpm() const {
    QMutexLocker locker(&m_mutex);
    if (!isValid()) {
        return -1;
    }
    return m_dBpm;
}

double BeatGrid::getBpmRange(double startSample, double stopSample) const {
    QMutexLocker locker(&m_mutex);
    if (!isValid()) {
        return -1;
    }
    return m_dBpm;
}

void BeatGrid::addBeat(double dBeatSample) {
    //QMutexLocker locker(&m_mutex);
    return;
}

void BeatGrid::removeBeat(double dBeatSample) {
    //QMutexLocker locker(&m_mutex);
    return;
}

void BeatGrid::moveBeat(double dBeatSample, double dNewBeatSample) {
    //QMutexLocker locker(&m_mutex);
    return;
}

void BeatGrid::translate(double dNumSamples) {
    QMutexLocker locker(&m_mutex);
    if (!isValid()) {
        return;
    }
    m_dFirstBeat += dNumSamples;
    locker.unlock();
    emit(updated());
}

void BeatGrid::scale(double dScalePercentage) {
    QMutexLocker locker(&m_mutex);
    if (!isValid()) {
        return;
    }
    m_dBpm *= dScalePercentage;
    locker.unlock();
    emit(updated());
}

void BeatGrid::slotTrackBpmUpdated(double dBpm) {
    QMutexLocker locker(&m_mutex);
    m_dBpm = dBpm;
}

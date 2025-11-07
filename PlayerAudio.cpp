#include "PlayerAudio.h"

PlayerAudio::PlayerAudio()
{
    formatManager.registerBasicFormats();
    resampleSource = std::make_unique<juce::ResamplingAudioSource>(&transportSource, false, 2);
}

PlayerAudio::~PlayerAudio()
{
}
bool PlayerAudio::loadFile(const juce::File& file)
{
    if (file.existsAsFile())
    {
        if (auto* reader = formatManager.createReaderFor(file))
        {
            transportSource.stop();
            transportSource.setSource(nullptr);
            readerSource.reset();

            readerSource = std::make_unique<juce::AudioFormatReaderSource>(reader, true);

            transportSource.setSource(readerSource.get(), 0, nullptr, reader->sampleRate);

            transportSource.start();
        }
    }
    return true;
}

void PlayerAudio::prepareToPlay(int samplesPerBlockExpected, double sampleRate)
{
    transportSource.prepareToPlay(samplesPerBlockExpected, sampleRate);
    if (resampleSource)
        resampleSource->prepareToPlay(samplesPerBlockExpected, sampleRate);
}

void PlayerAudio::getNextAudioBlock(const juce::AudioSourceChannelInfo& bufferToFill)
{
    if (resampleSource) {
        resampleSource->getNextAudioBlock(bufferToFill);
        if (segmentLooping && transportSource.isPlaying())
        {
            double currentPos = transportSource.getCurrentPosition();
            if (currentPos >= loopEndTime)
            {
                transportSource.setPosition(loopStartTime);
            }
        }
    }
    else {
        transportSource.getNextAudioBlock(bufferToFill);
        if (segmentLooping && transportSource.isPlaying())
        {
            double currentPos = transportSource.getCurrentPosition();
            if (currentPos >= loopEndTime)
            {
                transportSource.setPosition(loopStartTime);
            }
        }
    }
}

void PlayerAudio::releaseResources()
{
    transportSource.releaseResources();
    if (resampleSource)
        resampleSource->releaseResources();
}

void PlayerAudio::setSpeed(double speed)
{
    if (resampleSource)
        resampleSource->setResamplingRatio(speed);
}

void PlayerAudio::start()
{
    transportSource.start();
}

void PlayerAudio::setGain(float gain)
{
    transportSource.setGain(gain);
}

void PlayerAudio::setPosition(double pos) {
    transportSource.setPosition(pos);
}

double PlayerAudio::getCurrentPosition()
{
    return transportSource.getCurrentPosition();
}

double PlayerAudio::getPosition() const
{
    return transportSource.getCurrentPosition();
}

double PlayerAudio::getLength() const
{
    return transportSource.getLengthInSeconds();
}

void PlayerAudio::toggleMute()
{
    if (!Muted)
    {
        previous = transportSource.getGain();
        transportSource.setGain(0.0f);
        Muted = true;
    }
    else
    {
        transportSource.setGain(previous);
        Muted = false;
    }
}

void PlayerAudio::stop()
{
    transportSource.stop();
}

double PlayerAudio::getLengthInSeconds() const
{
    return transportSource.getLengthInSeconds();
}

void PlayerAudio::setLooping(bool shouldLoop)
{
    if (readerSource)
        readerSource->setLooping(shouldLoop);
}

void PlayerAudio::setLoopPoints(double startTime, double endTime)
{
    loopStartTime = startTime;
    loopEndTime = endTime;
    segmentLooping = true;
}

void PlayerAudio::clearLoopPoints()
{
    segmentLooping = false;
}

bool PlayerAudio::isSegmentLooping() const
{
    return segmentLooping;
}

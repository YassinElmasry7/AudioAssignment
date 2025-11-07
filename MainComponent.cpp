#include "MainComponent.h"

MainComponent::MainComponent()
{
    addAndMakeVisible(player1);
    addAndMakeVisible(player2);
    setSize(600, 350);
    setAudioChannels(0, 2);
}
MainComponent::~MainComponent()
{
    shutdownAudio();
}
void MainComponent::prepareToPlay(int samplesPerBlockExpected, double sampleRate)
{
    player1.prepareToPlay(samplesPerBlockExpected, sampleRate);
    player2.prepareToPlay(samplesPerBlockExpected, sampleRate);
}
void MainComponent::getNextAudioBlock(const juce::AudioSourceChannelInfo& bufferToFill)
{
    player1.getNextAudioBlock(bufferToFill);
    player2.getNextAudioBlock(bufferToFill);
}
void MainComponent::releaseResources()
{
    player1.releaseResources();
    player2.releaseResources();
}
void MainComponent::resized()
{
    auto theTotalArea = getLocalBounds();
    auto halfHeight = theTotalArea.getHeight() / 2;
    player1.setBounds(theTotalArea.removeFromTop(halfHeight));
    player2.setBounds(theTotalArea);
}

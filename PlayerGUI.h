#pragma once
#include "PlayerAudio.h"
#include <JuceHeader.h>
using namespace juce;
using namespace std;

class PlayerGUI : public juce::Component,
    public juce::Button::Listener,
    public juce::Slider::Listener,
    public juce::Timer
{
public:
    PlayerGUI();
    ~PlayerGUI() override;

    void prepareToPlay(int samplesPerBlockExpected, double sampleRate);
    void getNextAudioBlock(const juce::AudioSourceChannelInfo& bufferToFill);
    void releaseResources();

    void paint(juce::Graphics& g) override;
    void resized() override;
    void timerCallback() override;

private:

    PlayerAudio playerAudio;
    juce::TextButton loadButton{ "Load Files" };
    juce::TextButton restartButton{ "Restart" };
    juce::TextButton playButton{ "Play" };
    juce::TextButton pauseButton{ "Pause" };
    juce::TextButton gostartButton{ "Go to Start" };
    juce::TextButton goendButton{ "Go to End" };
    juce::TextButton muteButton{ "Mute" };
    juce::TextButton loopButton{ "Loop" };
    juce::TextButton setPointAButton{"Set Point A"};
    juce::TextButton setPointBButton{"Set Point B"};
    juce::TextButton clearLoopPointsButton{"Clear Loop"};
    juce::ListBox playlistBox;
    juce::TextButton addlistButton{ "Add to Playlist" };
    juce::TextButton removelistButton{ "Remove from Playlist" };
    juce::TextButton clearlistButton{ "Clear List" };

    double pointATime = 0.0;
    double pointBTime = 0.0;
    bool hasPointA = false;
    bool hasPointB = false;

    bool Looping = false;
    bool Playing = false;

    juce::Label titleLabel;
    juce::Label artistLabel;
    juce::Label durationLabel;
    juce::Label filenameLabel;
    juce::Label timeLabel;
    juce::Label loopPointsLabel;

    juce::Slider volumeSlider;
    juce::Slider positionSlider;
    juce::Slider speedSlider;

    juce::Array<juce::File> playlistFiles;

    juce::StringArray playlistDisplayNames;

    void updatePlaylist();
    int getSelectedPlaylistIndex() const;

    class PlaylistModel : public juce::ListBoxModel
    {
    public:
        PlaylistModel(PlayerGUI& owner) : owner(owner) {}

        int getNumRows() override;
        void paintListBoxItem(int rNum, juce::Graphics& g, int w, int h, bool rowSelected) override;
        void listBoxItemClicked(int r, const juce::MouseEvent& event) override;

    private:
        PlayerGUI& owner;
    };

    class ProgressBar : public juce::Component
    {
    public:
        ProgressBar(PlayerAudio& audio) : playerAudio(audio) {}

        void paint(juce::Graphics& g) override
        {
            double currentPos = playerAudio.getCurrentPosition();
            double totalLength = playerAudio.getLengthInSeconds();

            g.fillAll(juce::Colours::darkgrey);

            if (totalLength > 0)
            {
                g.setColour(juce::Colours::grey);
                g.fillRect(getLocalBounds());

                double progress = currentPos / totalLength;
                g.setColour(juce::Colours::lightblue);
                g.fillRect(0, 0, getWidth() * progress, getHeight());

                g.setColour(juce::Colours::red);
                g.drawLine(getWidth() * progress, 0, getWidth() * progress, getHeight(), 3.0f);

                g.setColour(juce::Colours::white);
                g.setFont(14.0f);

                int currentMinutes = static_cast<int>(currentPos) / 60;
                int currentSeconds = static_cast<int>(currentPos) % 60;
                juce::String currentTime = juce::String::formatted("%02d:%02d", currentMinutes, currentSeconds);
                g.drawText(currentTime, 5, 0, 50, getHeight(), juce::Justification::left);

                int totalMinutes = static_cast<int>(totalLength) / 60;
                int totalSeconds = static_cast<int>(totalLength) % 60;
                juce::String totalTime = juce::String::formatted("%02d:%02d", totalMinutes, totalSeconds);
                g.drawText(totalTime, getWidth() - 55, 0, 50, getHeight(), juce::Justification::right);

                juce::String percentText = juce::String::formatted("%.0f%%", progress * 100);
                g.drawText(percentText, getLocalBounds(), juce::Justification::centred);
            }
            else
            {
                g.setColour(juce::Colours::white);
                g.setFont(16.0f);
                g.drawText("No audio loaded", getLocalBounds(), juce::Justification::centred);
            }

            g.setColour(juce::Colours::white);
            g.drawRect(getLocalBounds(), 1);
        }

    private:
        PlayerAudio& playerAudio;
    };

    std::unique_ptr<ProgressBar> progressBar;
    std::unique_ptr<PlaylistModel> playlistModel;
    std::unique_ptr<juce::FileChooser>fileChooser;
    void buttonClicked(juce::Button* button) override;
    void sliderValueChanged(juce::Slider* slider) override;
    void displayMetadata(const juce::File& file);
    void updateLoopPointsLabel();

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(PlayerGUI)

};

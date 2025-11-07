#include "PlayerGUI.h"

PlayerGUI::PlayerGUI()
{
    progressBar = std::make_unique<ProgressBar>(playerAudio);
    addAndMakeVisible(progressBar.get());

    for (auto* btn : { &loadButton, &restartButton , &playButton, &pauseButton, &gostartButton, &goendButton, &muteButton, &loopButton,
        &addlistButton, &removelistButton, &clearlistButton, &setPointAButton, &setPointBButton, &clearLoopPointsButton  })
    {
        btn->addListener(this);
        btn->setColour(juce::TextButton::textColourOffId, juce::Colours::white);
        btn->setColour(juce::TextButton::buttonColourId, juce::Colours::purple);
        addAndMakeVisible(btn);
    }

    loadButton.setColour(juce::TextButton::buttonColourId, juce::Colours::purple);

    for (auto* lab : { &titleLabel, &artistLabel, &durationLabel, &filenameLabel, &timeLabel, &loopPointsLabel })
    {
        lab->setJustificationType(juce::Justification::left);
        lab->setColour(juce::Label::textColourId, juce::Colours::white);
        addAndMakeVisible(lab);
    }

    timeLabel.setText("00:00", juce::dontSendNotification);

    volumeSlider.setRange(0.0, 1.0, 0.01);
    volumeSlider.setValue(0.5);
    volumeSlider.addListener(this);
    volumeSlider.setSliderStyle(juce::Slider::SliderStyle::LinearVertical);
    addAndMakeVisible(volumeSlider);

    positionSlider.setRange(0.0, 1.0, 0.01);
    positionSlider.setValue(0.0);
    positionSlider.addListener(this);
    addAndMakeVisible(positionSlider);

    speedSlider.setRange(0.25, 2, 0.01);
    speedSlider.setValue(1.0);
    speedSlider.addListener(this);
    speedSlider.setSliderStyle(juce::Slider::SliderStyle::LinearVertical);
    addAndMakeVisible(speedSlider);

    playlistModel = std::make_unique<PlaylistModel>(*this);
    playlistBox.setModel(playlistModel.get());
    playlistBox.setMultipleSelectionEnabled(false);
    addAndMakeVisible(playlistBox);
    startTimerHz(30);
}

PlayerGUI::~PlayerGUI()
{
    stopTimer();
}

void PlayerGUI::prepareToPlay(int samplesPerBlockExpected, double sampleRate)
{
    playerAudio.prepareToPlay(samplesPerBlockExpected, sampleRate);
}

void PlayerGUI::getNextAudioBlock(const juce::AudioSourceChannelInfo& bufferToFill)
{
    playerAudio.getNextAudioBlock(bufferToFill);
}

void PlayerGUI::releaseResources()
{
    playerAudio.releaseResources();
}

void PlayerGUI::paint(juce::Graphics& g)
{
    g.fillAll(juce::Colours::black);
}

void PlayerGUI::timerCallback()
{
    progressBar->repaint();

    double currentPos = playerAudio.getCurrentPosition();
    double totalLength = playerAudio.getLengthInSeconds();

    if (totalLength > 0)
    {
        positionSlider.setValue(currentPos / totalLength, juce::dontSendNotification);

        int currentMin = static_cast<int>(currentPos) / 60;
        int currentSec = static_cast<int>(currentPos) % 60;
        int totalMin = static_cast<int>(totalLength) / 60;
        int totalSec = static_cast<int>(totalLength) % 60;

        juce::String timeText = juce::String::formatted("%02d:%02d / %02d:%02d", currentMin, currentSec, totalMin, totalSec);
        timeLabel.setText(timeText, juce::dontSendNotification);
    }
}

void PlayerGUI::resized()
{
    loadButton.setBounds(150, 20, 100, 40);
    restartButton.setBounds(265, 20, 100, 40);
    pauseButton.setBounds(380, 20, 100, 40);
    playButton.setBounds(495, 20, 100, 40);
    loopButton.setBounds(150, 80, 100, 40);
    gostartButton.setBounds(265, 80, 100, 40);
    goendButton.setBounds(380, 80, 100, 40);
    muteButton.setBounds(495, 80, 100, 40);
    addlistButton.setBounds(700, 100, 100, 30);
    removelistButton.setBounds(815, 100, 100, 30);
    clearlistButton.setBounds(930, 100, 100, 30);

    progressBar->setBounds(110, 180, 560, 40);

    titleLabel.setBounds(110, 270, 560, 40);
    artistLabel.setBounds(110, 270+25, 560, 40);
    durationLabel.setBounds(110, 270+50, 560, 40);
    filenameLabel.setBounds(110, 270+75, 560, 40);
    loopPointsLabel.setBounds(110, 250, 560, 40);

    volumeSlider.setBounds(650, 140, 60, 250);
    playlistBox.setBounds(720, 140, 500, 250); 

    setPointAButton.setBounds(110, 230, 100, 30);
    setPointBButton.setBounds(220, 230, 100, 30);
    clearLoopPointsButton.setBounds(330, 230, 100, 30);

    timeLabel.setBounds(350, 130, 80, 30);
    positionSlider.setBounds(100, 150, 560, 30);
    speedSlider.setBounds(20, 140, 60, 250);
}

void PlayerGUI::buttonClicked(juce::Button* button)
{
    if (button == &loadButton)
    {
        juce::FileChooser chooser("Select audio files...",
            juce::File{},
            "*.wav;*.mp3");

        fileChooser = std::make_unique<juce::FileChooser>(
            "Select an audio file...",
            juce::File{},
            "*.wav;*.mp3");

        fileChooser->launchAsync(
            juce::FileBrowserComponent::openMode | juce::FileBrowserComponent::canSelectFiles,
            [this](const juce::FileChooser& fc)
            {
                auto file = fc.getResult();
                if (file.existsAsFile()) {
                    playerAudio.loadFile(file);
                    displayMetadata(file);
                }
                double q = playerAudio.getLengthInSeconds();
                positionSlider.setRange(0.0, 1.0, 0.01);
                positionSlider.setValue(0.0);
            });
    }
    if (button == &addlistButton)
    {
        fileChooser = std::make_unique<juce::FileChooser>(
            "Select audio files for playlist...",
            juce::File{},
            "*.wav;*.mp3");

        fileChooser->launchAsync(
            juce::FileBrowserComponent::openMode | juce::FileBrowserComponent::canSelectMultipleItems,
            [this](const juce::FileChooser& fc)
            {
                auto results = fc.getResults();
                for (auto file : results)
                {
                    if (file.existsAsFile())
                    {
                        playlistFiles.add(file);
                    }
                }
                updatePlaylist();
            });
    }
    if (button == &removelistButton)
    {
        int selectedIndex = getSelectedPlaylistIndex();
        if (selectedIndex >= 0 && selectedIndex < playlistFiles.size())
        {
            playlistFiles.remove(selectedIndex);
            updatePlaylist();
        }
    }
    if (button == &clearlistButton)
    {
        playlistFiles.clear();
        updatePlaylist();
    }
    if (button == &playButton)
    {
        playerAudio.start();
    }
    if (button == &restartButton)
    {
        playerAudio.stop();
        playerAudio.setPosition(0.0);
        positionSlider.setValue(0.0);
    }
    if (button == &pauseButton)
    {
        playerAudio.stop();
    }
    if (button == &gostartButton)
    {
        playerAudio.setPosition(0.0);
        positionSlider.setValue(0.0);
        playerAudio.start();
    }
    if (button == &goendButton)
    {
        double end = playerAudio.getLengthInSeconds();
        playerAudio.setPosition(end);
        positionSlider.setValue(1.0);
        playerAudio.stop();
    }
    if (button == &muteButton) {
        playerAudio.toggleMute();

        if (playerAudio.Muted)
        {
            muteButton.setButtonText("Unmute");
            muteButton.setColour(juce::TextButton::buttonColourId, juce::Colours::red);
        }
        else
        {
            muteButton.setButtonText("Mute");
            muteButton.setColour(juce::TextButton::buttonColourId, juce::Colours::purple);
        }
    }
    if (button == &loopButton) {
        Looping = !Looping;
        playerAudio.setLooping(Looping);

        if (Looping)
        {
            loopButton.setButtonText("Looping");
            loopButton.setColour(juce::TextButton::buttonColourId, juce::Colours::lightgrey);
        }
        else
        {
            loopButton.setButtonText("Loop");
            loopButton.setColour(juce::TextButton::buttonColourId, juce::Colours::purple);
        }
    }
    if (button == &setPointAButton)
    {
        pointATime = playerAudio.getCurrentPosition();
        hasPointA = true;
        updateLoopPointsLabel();
    }
    if (button == &setPointBButton)
    {
        pointBTime = playerAudio.getCurrentPosition();
        hasPointB = true;
        updateLoopPointsLabel();

        if (hasPointA && hasPointB)
        {
            playerAudio.setLoopPoints(pointATime, pointBTime);
        }
    }
    if (button == &clearLoopPointsButton)
    {
        playerAudio.clearLoopPoints();
        hasPointA = false;
        hasPointB = false;
        updateLoopPointsLabel();
    }
}

void PlayerGUI::updateLoopPointsLabel()
{
    if (hasPointA && hasPointB)
    {
        loopPointsLabel.setText("Loop: A=" + juce::String(pointATime, 1) +
            "s, B=" + juce::String(pointBTime, 1) + "s",
            juce::dontSendNotification);
    }
    else if (hasPointA)
    {
        loopPointsLabel.setText("Loop: A=" + juce::String(pointATime, 1) + "s",
            juce::dontSendNotification);
    }
    else
    {
        loopPointsLabel.setText("Loop: Off", juce::dontSendNotification);
    }
}

void PlayerGUI::sliderValueChanged(juce::Slider* slider)
{
    if (slider == &volumeSlider)
    {
        playerAudio.setGain((float)slider->getValue());
    }
    if (slider == &positionSlider)
    {
        double totalLength = playerAudio.getLengthInSeconds();
        if (totalLength > 0)
        {
            double actualPosition = slider->getValue() * totalLength;
            playerAudio.setPosition(actualPosition);
        }
    }
    if (slider == &speedSlider)
    {
        playerAudio.setSpeed((float)slider->getValue());
    }
}
int PlayerGUI::PlaylistModel::getNumRows()
{
    return owner.playlistFiles.size();
}

int PlayerGUI::getSelectedPlaylistIndex() const
{
    return playlistBox.getSelectedRow();
}

void PlayerGUI::PlaylistModel::paintListBoxItem(int rNum, juce::Graphics& g, int w, int h, bool rowSelected)
{
    if (rowSelected)
        g.fillAll(juce::Colours::blue);
    else
        g.fillAll(juce::Colours::white);

    g.setColour(juce::Colours::black);

    if (rNum < owner.playlistDisplayNames.size())
    {
        g.drawText(owner.playlistDisplayNames[rNum],
            10, 0, w - 10, h,
            juce::Justification::centredLeft);
    }
}

void PlayerGUI::PlaylistModel::listBoxItemClicked(int r, const juce::MouseEvent& event)
{
    if (r < owner.playlistFiles.size())
    {
        owner.playerAudio.loadFile(owner.playlistFiles[r]);
        owner.displayMetadata(owner.playlistFiles[r]);
        owner.positionSlider.setRange(0.0, 1.0, 0.01);
        owner.positionSlider.setValue(0.0);
        owner.progressBar->repaint();
    }
}

void PlayerGUI::updatePlaylist()
{
    playlistDisplayNames.clear();

    for (int i = 0; i < playlistFiles.size(); ++i)
    {
        juce::String displayName = juce::String(i + 1) + ". " + playlistFiles[i].getFileName();
        playlistDisplayNames.add(displayName);
    }

    playlistBox.updateContent();
    repaint();
}

void PlayerGUI::displayMetadata(const juce::File& file)
{
    filenameLabel.setText("Filename: " + file.getFileName(), juce::dontSendNotification);

    juce::AudioFormatManager formatManager;
    formatManager.registerBasicFormats();

    if (auto* reader = formatManager.createReaderFor(file))
    {
        juce::String title = reader->metadataValues.getValue("Title", "");
        juce::String artist = reader->metadataValues.getValue("Artist", "");
        juce::String album = reader->metadataValues.getValue("Album", "");

        if (title.isEmpty())
            title = file.getFileNameWithoutExtension();

        if (artist.isEmpty())
            artist = "Unknown Artist";

        double durationInSeconds = reader->lengthInSamples / reader->sampleRate;
        int minutes = static_cast<int>(durationInSeconds) / 60;
        int seconds = static_cast<int>(durationInSeconds) % 60;
        juce::String duration = juce::String::formatted("%02d:%02d", minutes, seconds);

        titleLabel.setText("Title: " + title, juce::dontSendNotification);
        artistLabel.setText("Artist: " + artist, juce::dontSendNotification);
        durationLabel.setText("Duration: " + duration, juce::dontSendNotification);
        timeLabel.setText(duration, juce::dontSendNotification);

        delete reader;
    }
}
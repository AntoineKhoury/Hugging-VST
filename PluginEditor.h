#include <JuceHeader.h>
#include "PluginProcessor.h"


class NewProjectAudioProcessorEditor : public juce::AudioProcessorEditor {
public:
    NewProjectAudioProcessorEditor(NewProjectAudioProcessor&);
    ~NewProjectAudioProcessorEditor() override;

    void paint(juce::Graphics&) override;
    void resized() override;
    void sendMessage(const std::string &message);
    void downloadFile(const std::string& url);
private:
    NewProjectAudioProcessor& audioProcessor;
    juce::TextEditor inputText; // Text box for user input
    juce::Label statusLabel;
    juce::Label downloadurl;
    juce::TextButton validateButton; // Button to validate the choice of text
    gin::AsyncWebsocket websocket;

        

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(NewProjectAudioProcessorEditor)
};



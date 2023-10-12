#include "PluginProcessor.h"
#include "PluginEditor.h"
#include <iostream>
#include <cstdlib>
using namespace std;


NewProjectAudioProcessorEditor::NewProjectAudioProcessorEditor(NewProjectAudioProcessor& p)
: AudioProcessorEditor(&p), audioProcessor(p),websocket(juce::URL("wss://facebook-musicgen.hf.space/queue/join"))
{
    addAndMakeVisible(inputText);
    inputText.setMultiLine(false);
    inputText.setReturnKeyStartsNewLine(false);
    inputText.setReadOnly(false);
    inputText.setScrollbarsShown(true);
    inputText.setCaretVisible(true);
    inputText.setPopupMenuEnabled(true);

    addAndMakeVisible(validateButton);
    validateButton.setButtonText("Send");
    // Set the connection when the button is pressed
    validateButton.onClick = [this] {
    juce::String text = inputText.getText();
    sendMessage(text.toStdString());
    };
        
    addAndMakeVisible(statusLabel);
    statusLabel.setFont(juce::Font(16.0f));
    statusLabel.setText("Connection Status: Disconnected", juce::NotificationType::dontSendNotification);
                
    statusLabel.setColour (juce::Label::textColourId, juce::Colours::black);
    
    addAndMakeVisible(downloadurl);
    downloadurl.setFont(juce::Font(16.0f));
    downloadurl.setText("Download URL: None", juce::NotificationType::dontSendNotification);
                
    downloadurl.setColour (juce::Label::textColourId, juce::Colours::black);

    setSize(600, 500);
}

NewProjectAudioProcessorEditor::~NewProjectAudioProcessorEditor() {

}

void NewProjectAudioProcessorEditor::paint(juce::Graphics& g) {
    g.fillAll(getLookAndFeel().findColour(juce::ResizableWindow::backgroundColourId));
    g.setColour(juce::Colours::white);
    g.setFont(15.0f);
    g.drawFittedText("Request audio from the MusicGen Facebook model made available by HuggingFace !", getLocalBounds(), juce::Justification::centred, 1);

}

void NewProjectAudioProcessorEditor::resized() {
    inputText.setBounds(10, 10, getWidth() - 20, 30);
    validateButton.setBounds((getWidth() - 100) * 0.5, 50, 100, 30);
    statusLabel.setBounds(10, 90, getWidth() - 20, 60);
    downloadurl.setBounds(10, 130, getWidth() - 20, 60);
}

void NewProjectAudioProcessorEditor::sendMessage(const std::string &message) {
    // Initialize the WebSocket if it's not already created
    statusLabel.setText("Trying to connect", juce::NotificationType::dontSendNotification);
    std::string hash = "7k2d"+std::to_string(rand()%1000)+"t47s";
    cout << "hash created ";
    cout << hash +"\n";
    websocket.onConnect = [this]{
        statusLabel.setText("Connected", juce::NotificationType::dontSendNotification);
           };
    websocket.onDisconnect = [this]{
               
    };
    websocket.onText = [this,message,hash] (const juce::String& txt){
        statusLabel.setText(txt,juce::NotificationType::dontSendNotification);
        juce::var msg_data = juce::JSON::parse(txt);
        cout << msg_data["msg"].toString();
        if (msg_data["msg"] == "send_hash") {
            juce::DynamicObject response;
            juce::String response_str="{\"fn_index\": 0, \"session_hash\": \""+hash+"\"}";
            cout << response_str.toStdString()+"\n";
            websocket.send(response_str);
        }
        
        else if (msg_data["msg"] == "send_data") {
            cout << "send data received";
            juce::String response_str = "{\"data\": [\"" + juce::String(message) + "\", null], \"event_data\": null, \"fn_index\": 0, \"session_hash\": \""+hash+"\"}";
            downloadurl.setText(response_str, juce::NotificationType::dontSendNotification);
            websocket.send(response_str);
        }
        
        else if (msg_data["msg"] == "process_starts"){
            cout << "process started. waiting for end of process";
            downloadurl.setText("Process has started",juce::NotificationType::dontSendNotification);
            
        }
         
        else if (msg_data["msg"] == "process_completed") {
            
            // Extract the file path
            std::string file_path = juce::String(msg_data["output"]["data"][0][0]["name"]).toStdString();
            
            // Construct the download URL
            std::string download_url = "https://facebook-musicgen.hf.space/file=" + file_path;
            downloadurl.setText(download_url,juce::NotificationType::dontSendNotification);
            downloadFile(download_url);
             
        }
        
    
    };

    websocket.connect();
}
void NewProjectAudioProcessorEditor::downloadFile(const std::string& url)
{
    juce::URL fileURL(url);
    juce::File destinationFile("output.mp4");

    // Start the download asynchronously
    fileURL.downloadToFile(destinationFile);
}

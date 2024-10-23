## JuceConnector
* Experimental Unreal plugin for testing whether VST3-based plugins can be used in the audio processing under the Unreal Engine environment.
* This plugin uses the [JUCE framework](https://github.com/juce-framework/JUCE) internally to host VST3 plugins in the Unreal Editor.

## Development Environment
* Unreal Engine 5.4 (with Windows)
* JUCE v8.0.2

⚠️ Other platforms and versions haven't been tested.

## Integration Steps
1. Create a new folder named 'JuceConnector' in your Unreal project's plugin folder.
2. Copy all contents of this repository to the folder you created.
3. Create a folder following the 'ThirdParty/JUCE' path under the 'JuceConnector' plugin folder.
    * For example, if the plugin is under 'C:\MyProject\Plugins\JuceConnector', there should be a folder at 'C:\MyProject\Plugins\JuceConnector\ThirdParty\JUCE'.
4. Copy all JUCE modules provided by default to the folder created in step 3.
    * All standard JUCE modules like 'juce_audio_processor' and 'juce_graphics' must be copied to the folder.

## Usage
ℹ️ **The [GainPluginDemo](https://github.com/juce-framework/JUCE/tree/master/examples/Plugins) from JUCE is used in the example images.**
### Plugin Asset
1. Open the project settings window, and add folders where VST3 plugins are located in the local computer.
    * The setting should be in the 'Juce Connector > Scan Settings' route.
  
    ![image](https://github.com/user-attachments/assets/195ea8b6-14dd-4a96-a042-c583678ff09e)

2. From the Content Browser, create a new JucePluginAsset. It requires you to select a VST3 plugin you would like to use.

    ![image](https://github.com/user-attachments/assets/e67e6a13-9159-4c12-b38a-c6a323881b8d)

3. With the JucePluginAsset, you can edit the exposed parameters or open the plugin Editor by clicking the 'Open' button.
   
    ![image](https://github.com/user-attachments/assets/1e73544d-8986-462b-89cd-898b47d90032)


### Audio effect units
* SubmixEffect
    1. Create a SubmixEffectPreset asset, and select the JucePluginSubmixEffectPreset as a base SubmixEffect class.

    ![image](https://github.com/user-attachments/assets/37db4bfc-5a0a-4a21-b79f-9e66e52df84d)

    2. Open the created SubmixEffectPreset and plug a JucePluginAsset into the SubmixEffectPreset.
    3. When the SubmixEffectPreset is applied to a submix effect chain, signals start to be processed by using a VST3 plugin you selected.

* SourceEffect
    1. Create a SourceEffectPreset asset, and select the JucePluginSourceEffectPreset as a base SourceEffect class.

    ![image](https://github.com/user-attachments/assets/35f79edd-dd86-40a6-a768-240f82d272f0)

    2. Open the created SourceEffectPreset and plug a JucePluginAsset into the SourceEffectPreset.
    3. When the SourceEffectPreset is added to a SourceEffectPresetChain, signals start to be processed by using a VST3 plugin you selected.
       
* Metasound Node
    1. Search the 'Process (JUCE)' node in the Metasound editor.
    2. When an audio buffer passes through the node, the buffer will be processed by using a VST3 plugin you selected.

    ![image](https://github.com/user-attachments/assets/3d450271-a4db-4525-a6a7-963447e2b928)


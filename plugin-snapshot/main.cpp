// 使い方: plugin-snapshot <out.png> [--scale S] [--width W --height H] [paramID=value ...]
//         plugin-snapshot --list
// valueはパラメータの実際の単位(dB、%など)。Bool/Choiceは0,1,2...のインデックス。
#include <JuceHeader.h>

juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter();

static juce::RangedAudioParameter* findParameter (juce::AudioProcessor& p, const juce::String& id)
{
    for (auto* param : p.getParameters())
        if (auto* ranged = dynamic_cast<juce::RangedAudioParameter*> (param))
            if (ranged->getParameterID() == id)
                return ranged;

    return nullptr;
}

int main (int argc, char** argv)
{
    juce::ScopedJuceInitialiser_GUI init;
    std::unique_ptr<juce::AudioProcessor> processor (createPluginFilter());

    juce::StringArray args;
    for (int i = 1; i < argc; ++i)
        args.add (argv[i]);

    if (args.isEmpty() || args[0] == "--list")
    {
        std::cout << "paramID\tname\tmin\tmax\tdefault\n";
        for (auto* param : processor->getParameters())
            if (auto* r = dynamic_cast<juce::RangedAudioParameter*> (param))
                std::cout << r->getParameterID() << '\t' << r->getName (64) << '\t'
                          << r->getNormalisableRange().start << '\t' << r->getNormalisableRange().end << '\t'
                          << r->convertFrom0to1 (r->getDefaultValue()) << '\n';
        return args.isEmpty() ? 1 : 0;
    }

    const auto outFile = juce::File::getCurrentWorkingDirectory().getChildFile (args[0]);
    float scale = 1.0f;
    int width = 0, height = 0;

    for (int i = 1; i < args.size(); ++i)
    {
        const auto& a = args[i];

        if (a == "--scale")       scale  = args[++i].getFloatValue();
        else if (a == "--width")  width  = args[++i].getIntValue();
        else if (a == "--height") height = args[++i].getIntValue();
        else if (a.contains ("="))
        {
            const auto id = a.upToFirstOccurrenceOf ("=", false, false);
            auto* param = findParameter (*processor, id);

            if (param == nullptr)
            {
                std::cerr << "unknown parameter: " << id << " (--list で一覧を表示)\n";
                return 1;
            }

            param->setValueNotifyingHost (param->convertTo0to1 (a.fromFirstOccurrenceOf ("=", false, false).getFloatValue()));
        }
        else
        {
            std::cerr << "unknown argument: " << a << '\n';
            return 1;
        }
    }

    std::unique_ptr<juce::AudioProcessorEditor> editor (processor->createEditorIfNeeded());

    if (editor == nullptr)
    {
        std::cerr << "このプラグインはエディタを持っていません\n";
        return 1;
    }

    if (width > 0 && height > 0)
        editor->setSize (width, height);
    else if (scale != 1.0f)
        editor->setSize (juce::roundToInt ((float) editor->getWidth() * scale),
                         juce::roundToInt ((float) editor->getHeight() * scale));

    const auto image = editor->createComponentSnapshot (editor->getLocalBounds(), true, 1.0f);

    outFile.deleteFile();
    outFile.getParentDirectory().createDirectory();
    {
        juce::FileOutputStream stream (outFile);
        if (! stream.openedOk() || ! juce::PNGImageFormat().writeImageToStream (image, stream))
        {
            std::cerr << "failed to write: " << outFile.getFullPathName() << '\n';
            return 1;
        }
    }

    std::cout << outFile.getFullPathName() << " (" << image.getWidth() << "x" << image.getHeight() << ")\n";
    editor.reset();
    return 0;
}

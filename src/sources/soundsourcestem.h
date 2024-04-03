#pragma once

#include <QJsonObject>
#include <QList>
#include <QString>

#include "sources/soundsourceffmpeg.h"
#include "sources/soundsourceprovider.h"
#include "util/samplebuffer.h"

namespace mixxx {

/// @brief Handle a single stem stem embedded in a stem file
class SoundSourceSingleSTEM : public SoundSourceFFmpeg {
  public:
    // streamIdx is the FFmpeg stream id, which may different than stemIdx + 1
    // because STEm may contain other non audio stream
    explicit SoundSourceSingleSTEM(const QUrl& url, unsigned int streamIdx);

  protected:
    OpenResult tryOpen(
            OpenMode mode,
            const OpenParams& params) override;

  private:
    unsigned int m_streamIdx;
};

/// @brief Handle a stem file, composed of multiple audio channel. Can open in
/// stereo or in stem (4 x stereo)
class SoundSourceSTEM : public SoundSource {
  public:
    explicit SoundSourceSTEM(const QUrl& url);

    class Manifest {
      public:
        Manifest()
                : m_streams(0), m_version() {
        }

        bool isValid() const;
        static Manifest fromJson(const QJsonObject& json);

      private:
        struct Stream {
            QColor color;
            QString name;
        };

        Manifest(uint version, const QList<Stream>& streams)
                : m_streams(streams), m_version(version) {
        }

        QList<Stream> m_streams;
        // TODO(XXX): store the DSP parameters for post processing effect
        uint m_version;
    };

    void close() override;

  private:
    Manifest m_manifest;
    // Contains each stem source, or the main mix if opened in stereo mode
    std::vector<std::unique_ptr<SoundSourceSingleSTEM>> m_pStereoStreams;
    SampleBuffer m_buffer;

  protected:
    OpenResult tryOpen(
            OpenMode mode,
            const OpenParams& params) override;

    ReadableSampleFrames readSampleFramesClamped(
            const WritableSampleFrames& sampleFrames) override;
};

class SoundSourceProviderSTEM : public SoundSourceProvider {
  public:
    static const QString kDisplayName;

    ~SoundSourceProviderSTEM() override = default;

    QString getDisplayName() const override {
        return kDisplayName + QChar(' ') + getVersionString();
    }

    QStringList getSupportedFileTypes() const override;

    SoundSourceProviderPriority getPriorityHint(
            const QString& supportedFileType) const override;

    SoundSourcePointer newSoundSource(const QUrl& url) override {
        return newSoundSourceFromUrl<SoundSourceSTEM>(url);
    }

    QString getVersionString() const;
};

} // namespace mixxx

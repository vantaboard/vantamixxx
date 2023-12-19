#pragma once

#include <util/assert.h>

#include <QDebug>
#include <QDir>
#include <QDomElement>
#include <QHash>
#include <QList>
#include <QSharedPointer>
#include <QString>
#include <QtGlobal>
#include <memory>
#include <vector>

#include "controllers/legacycontrollersettings.h"
#include "controllers/legacycontrollersettingslayout.h"
#include "defs_urls.h"
#include "preferences/usersettings.h"

#define CONTROLLER_SETTINGS_PREFERENCE_GROUP_KEY "[ControllerSettings_%1_%2]"
#define CONTROLLER_SETTINGS_SETTING_PATH_SUBST "%SETTING_PATH"
#define CONTROLLER_SETTINGS_RESOURCE_PATH_SUBST "%RESOURCE_PATH"

/// This class represents a controller mapping, containing the data elements that
/// make it up.
class LegacyControllerMapping {
  public:
    LegacyControllerMapping()
            : m_bDirty(false) {
    }
    LegacyControllerMapping(const LegacyControllerMapping& other)
            : m_productMatches(other.m_productMatches),
              m_bDirty(other.m_bDirty),
              m_deviceId(other.m_deviceId),
              m_filePath(other.m_filePath),
              m_name(other.m_name),
              m_author(other.m_author),
              m_description(other.m_description),
              m_forumlink(other.m_forumlink),
              m_manualPage(other.m_manualPage),
              m_wikilink(other.m_wikilink),
              m_schemaVersion(other.m_schemaVersion),
              m_mixxxVersion(other.m_mixxxVersion),
              m_settings(other.m_settings),
              m_settingsLayout(other.m_settingsLayout.get() != nullptr
                              ? other.m_settingsLayout->clone()
                              : nullptr),
              m_scripts(other.m_scripts) {
    }
    virtual ~LegacyControllerMapping() = default;

    virtual std::shared_ptr<LegacyControllerMapping> clone() const = 0;

    struct ScriptFileInfo {
        ScriptFileInfo()
                : builtin(false) {
        }

        QString name;
        QString functionPrefix;
        QFileInfo file;
        bool builtin;
    };

    /// Adds a script file to the list of controller scripts for this mapping.
    /// @param filename Name of the script file to add
    /// @param functionprefix The script's function prefix (or empty string)
    /// @param file A FileInfo object pointing to the script file
    /// @param builtin If this is true, the script won't be written to the XML
    void addScriptFile(const QString& name,
            const QString& functionprefix,
            const QFileInfo& file,
            bool builtin = false) {
        ScriptFileInfo info;
        info.name = name;
        info.functionPrefix = functionprefix;
        info.file = file;
        info.builtin = builtin;
        m_scripts.append(info);
        setDirty(true);
    }

    /// Adds a setting option to the list of setting option for this mapping.
    /// The option added must be a valid option.
    /// @param option The option to add
    /// @return whether or not the setting was added successfully.
    bool addSetting(std::shared_ptr<AbstractLegacyControllerSetting> option) {
        VERIFY_OR_DEBUG_ASSERT(option->valid()) {
            return false;
        }
        for (const auto& setting : qAsConst(m_settings)) {
            if (*setting == *option) {
                qWarning() << "Mapping setting duplication detected for "
                              "setting with name"
                           << option->variableName()
                           << ". Keeping the first occurrence.";
                return false;
            }
        }
        m_settings.append(option);
        return true;
    }

    /// @brief Set a setting layout as they should be perceived when edited in
    /// the preference dialog.
    /// @param layout The layout root element
    void setSettingLayout(std::unique_ptr<LegacyControllerSettingsLayoutElement>&& layout) {
        VERIFY_OR_DEBUG_ASSERT(layout.get()) {
            return;
        }
        m_settingsLayout = std::move(layout);
    }

    const QList<ScriptFileInfo>& getScriptFiles() const {
        return m_scripts;
    }

    const QList<std::shared_ptr<AbstractLegacyControllerSetting>>& getSettings() const {
        return m_settings;
    }

    LegacyControllerSettingsLayoutElement* getSettingsLayout() {
        return m_settingsLayout.get();
    }

    void setDirty(bool bDirty) {
        m_bDirty = bDirty;
    }

    bool isDirty() const {
        return m_bDirty;
    }

    void setDeviceId(const QString& id) {
        m_deviceId = id;
        setDirty(true);
    }

    QString deviceId() const {
        return m_deviceId;
    }

    void setFilePath(const QString& filePath) {
        m_filePath = filePath;
        setDirty(true);
    }

    QString filePath() const {
        return m_filePath;
    }

    QDir dirPath() const {
        return QFileInfo(filePath()).absoluteDir();
    }

    void setName(const QString& name) {
        m_name = name;
        setDirty(true);
    }

    QString name() const {
        return m_name;
    }

    void setAuthor(const QString& author) {
        m_author = author;
        setDirty(true);
    }

    inline QString author() const {
        return m_author;
    }

    inline void setDescription(const QString& description) {
        m_description = description;
        setDirty(true);
    }

    inline QString description() const {
        return m_description;
    }

    inline void setForumLink(const QString& forumlink) {
        m_forumlink = forumlink;
        setDirty(true);
    }

    inline QString forumlink() const {
        return m_forumlink;
    }

    void setManualPage(const QString& manualPage) {
        m_manualPage = manualPage;
        setDirty(true);
    }

    QString manualPage() const {
        return m_manualPage;
    }

    QString manualLink() const {
        QString page = manualPage();
        if (page.isEmpty()) {
            return {};
        }

        return MIXXX_MANUAL_CONTROLLERMANUAL_PREFIX + page + MIXXX_MANUAL_CONTROLLERMANUAL_SUFFIX;
    }

    inline void setWikiLink(const QString& wikilink) {
        m_wikilink = wikilink;
        setDirty(true);
    }

    inline QString wikilink() const {
        return m_wikilink;
    }

    inline void setSchemaVersion(const QString& schemaVersion) {
        m_schemaVersion = schemaVersion;
        setDirty(true);
    }

    inline QString schemaVersion() const {
        return m_schemaVersion;
    }

    inline void setMixxxVersion(const QString& mixxxVersion) {
        m_mixxxVersion = mixxxVersion;
        setDirty(true);
    }

    inline QString mixxxVersion() const {
        return m_mixxxVersion;
    }

    inline void addProductMatch(const QHash<QString, QString>& match) {
        m_productMatches.append(match);
        setDirty(true);
    }

    virtual bool saveMapping(const QString& filename) const = 0;

    virtual bool isMappable() const = 0;

    void loadSettings(const QFileInfo& mappingFile,
            UserSettingsPointer pConfig,
            const QString& controllerName) const;
    void saveSettings(const QFileInfo& mappingFile,
            UserSettingsPointer pConfig,
            const QString& controllerName) const;

    // Optional list of controller device match details
    QList<QHash<QString, QString>> m_productMatches;

  private:
    bool m_bDirty;

    QString m_deviceId;
    QString m_filePath;
    QString m_name;
    QString m_author;
    QString m_description;
    QString m_forumlink;
    QString m_manualPage;
    QString m_wikilink;
    QString m_schemaVersion;
    QString m_mixxxVersion;

    QList<std::shared_ptr<AbstractLegacyControllerSetting>> m_settings;
    std::unique_ptr<LegacyControllerSettingsLayoutElement> m_settingsLayout;
    QList<ScriptFileInfo> m_scripts;
};

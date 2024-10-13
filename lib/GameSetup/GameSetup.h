#ifndef GAMESETUP_H
#define GAMESETUP_H
#include <Arduino.h>  // Include this line if you're using Arduino
#include <map>
#include <vector>

#include <ArduinoJson.h>
#include <Preferences.h>
// #include "weapon_desc.h"

typedef unsigned short ushort;

namespace game_management {

/****************************************************************************************************************************************************************************************/

enum class GAMEMODES { FREE_FOR_ALL, TEAM_DEATH_MATCH, CONQUEST, DOMINATION, TEAM_DOMINATION };
static std::map<GAMEMODES, std::pair<String, bool>> GameModes = {{GAMEMODES::FREE_FOR_ALL, {"FREE_FOR_ALL", false}},
                                                                 {GAMEMODES::TEAM_DEATH_MATCH, {"TEAM_DEATH_MATCH", true}},
                                                                 {GAMEMODES::CONQUEST, {"CONQUEST", true}},
                                                                 {GAMEMODES::DOMINATION, {"DOMINATION", true}},
                                                                 {GAMEMODES::TEAM_DOMINATION, {"TEAM_DOMINATION", true}}};

/****************************************************************************************************************************************************************************************/

enum class LAUNCHTRIGGER { TIMER, SPAWNPOINT, UNKNOWN_LAUNCHTRIGGER };
static std::map<LAUNCHTRIGGER, String> LaunchTriggers = {{LAUNCHTRIGGER::TIMER, "TIMER"}, {LAUNCHTRIGGER::SPAWNPOINT, "SPAWNPOINT"}, {LAUNCHTRIGGER::UNKNOWN_LAUNCHTRIGGER, "UNKNOWN_LAUNCHTRIGGER"}};

/****************************************************************************************************************************************************************************************/

enum class IRSIGNALTYPE { TAG, UNTAG, GRENADE, AMMUNITION, MEDKIT, BASE, SPAWNPOINT, CAPTUREPOINT };

static std::map<IRSIGNALTYPE, String> IrSignalType = {{IRSIGNALTYPE::TAG, "TAG"},       {IRSIGNALTYPE::UNTAG, "UNTAG"}, {IRSIGNALTYPE::GRENADE, "GRENADE"},       {IRSIGNALTYPE::AMMUNITION, "AMMUNITION"},
                                                      {IRSIGNALTYPE::MEDKIT, "MEDKIT"}, {IRSIGNALTYPE::BASE, "BASE"},   {IRSIGNALTYPE::SPAWNPOINT, "SPAWNPOINT"}, {IRSIGNALTYPE::CAPTUREPOINT, "CAPTUREPOINT"}};

/****************************************************************************************************************************************************************************************/

enum class DEVICETYPE { VEST, WEAPON, BASE, INGAME, UNKNOWN_DEVIDE };
static std::map<DEVICETYPE, String> DeviceTypes = {{DEVICETYPE::VEST, "VEST"}, {DEVICETYPE::WEAPON, "WEAPON"}, {DEVICETYPE::BASE, "BASE"}, {DEVICETYPE::INGAME, "INGAME"}, {DEVICETYPE::UNKNOWN_DEVIDE, "UNKNOWN_DEVIDE"}};

/****************************************************************************************************************************************************************************************/

enum class STOPTRIGGER { TIME, SCORE };
static std::map<STOPTRIGGER, String> StopTriggers = {{STOPTRIGGER::TIME, "TIME"}, {STOPTRIGGER::SCORE, "SCORE"}};

/****************************************************************************************************************************************************************************************/

enum class WEAPONTYPE { RIFLE, SNIPER, SHOTGUN, PISTOL, REVOLVER, SUBMACHINEGUN, MACHINEGUN };
static std::map<WEAPONTYPE, String> WeaponTypesMap = {{WEAPONTYPE::RIFLE, "RIFLE"},          {WEAPONTYPE::SNIPER, "SNIPER"},     {WEAPONTYPE::SHOTGUN, "SHOTGUN"},
                                                      {WEAPONTYPE::PISTOL, "PISTOL"},        {WEAPONTYPE::REVOLVER, "REVOLVER"}, {WEAPONTYPE::SUBMACHINEGUN, "SUBMACHINEGUN"},
                                                      {WEAPONTYPE::MACHINEGUN, "MACHINEGUN"}};

/****************************************************************************************************************************************************************************************/

enum class DEVICESTATUS {
    DETACHED,
    REGISTERED,
    READY,  // Weapon is linked on the player side
    ACTIVE,
    INACTIVE,
    SENDING,
    RELOADING,
    RESET
};

static std::map<DEVICESTATUS, String> DeviceStatus = {{DEVICESTATUS::DETACHED, "DETACHED"}, {DEVICESTATUS::REGISTERED, "REGISTERED"}, {DEVICESTATUS::READY, "READY"},         {DEVICESTATUS::ACTIVE, "ACTIVE"},
                                                      {DEVICESTATUS::INACTIVE, "INACTIVE"}, {DEVICESTATUS::SENDING, "SENDING"},       {DEVICESTATUS::RELOADING, "RELOADING"}, {DEVICESTATUS::RESET, "RESET"}};

// struct Device {
//     DEVICETYPE deviceType;
//     DEVICESTATUS deviceStatus;

//     Device() = default;

//     // Constructor
//     Device(DEVICETYPE _deviceType, DEVICESTATUS _deviceStatus) : deviceType(_deviceType), deviceStatus(_deviceStatus) {}
// };

/****************************************************************************************************************************************************************************************/

enum class GAMESTATE { SETUP, SETUP_COMPLETE, RUNNING, PAUSED, RESUME, GAMEOVER, DONE };
static std::map<GAMESTATE, String> GameStates = {
    {GAMESTATE::SETUP, "SETUP"}, {GAMESTATE::SETUP_COMPLETE, "SETUP_COMPLETE"}, {GAMESTATE::RUNNING, "RUNNING"}, {GAMESTATE::PAUSED, "PAUSED"}, {GAMESTATE::RESUME, "RESUME"}, {GAMESTATE::GAMEOVER, "GAMEOVER"}, {GAMESTATE::DONE, "DONE"}};

/****************************************************************************************************************************************************************************************/

enum class PLAYERSTATE {
    DETACHED,    // Initial state before the player connect to the HQ
    CONNECTED,   // Player receive the registed message (resRegister)
    REGISTERED,  // HQ added weapon to player (resWeaponAdded)
    WEAPEN_LOADED,
    READY,    // Weapon is linked on the player side
    SETUP,    // HQ have sent the setup message message (gameSetup)
    STARTED,  // HQ have sent the start message (gameStart)
    PAUSED,
    ALIVE,
    HURT,
    DEAD_COOLDOWN,
    DEAD,
    RESPAWNING,
    SPECTATOR,
    EVALUATING,
    EVALUATED
};
static std::map<PLAYERSTATE, String> PlayerStates = {{PLAYERSTATE::DETACHED, "DETACHED"},
                                                     {PLAYERSTATE::CONNECTED, "CONNECTED"},
                                                     {PLAYERSTATE::REGISTERED, "REGISTERED"},
                                                     {PLAYERSTATE::WEAPEN_LOADED, "WEAPEN_LOADED"},
                                                     {PLAYERSTATE::READY, "READY"},
                                                     {PLAYERSTATE::SETUP, "SETUP"},
                                                     {PLAYERSTATE::STARTED, "STARTED"},
                                                     {PLAYERSTATE::PAUSED, "PAUSED"},
                                                     {PLAYERSTATE::ALIVE, "ALIVE"},
                                                     {PLAYERSTATE::HURT, "HURT"},
                                                     {PLAYERSTATE::DEAD_COOLDOWN, "DEAD_COOLDOWN"},
                                                     {PLAYERSTATE::DEAD, "DEAD"},
                                                     {PLAYERSTATE::RESPAWNING, "RESPAWNING"},
                                                     {PLAYERSTATE::SPECTATOR, "SPECTATOR"},
                                                     {PLAYERSTATE::EVALUATING, "EVALUATING"},
                                                     {PLAYERSTATE::EVALUATED, "EVALUATED"}};

/****************************************************************************************************************************************************************************************/

struct Player {
    uint32_t playerId = 999;
    uint32_t team = 0;
    String name = "";
    PLAYERSTATE playerState = PLAYERSTATE::DETACHED;
    PLAYERSTATE previousState;
    String playerAddr = "";
    std::vector<String> weaponAddrs = {};

    uint32_t score = 0;    // score of player
    uint32_t kills = 0;    // amount of kills
    uint32_t deaths = 0;   // amount of deaths
    uint32_t cpoint = 0;   // amount of captured points
    uint32_t gpoint = 0;   // points received for certain actions (e.g. capturing a point)
    uint32_t assists = 0;  // amount of assists
    uint32_t hits = 0;     // amount of assists

    int maxHealth = 100;
    int minHealth = 0;
    int health = 100;

    bool localRespawn = true;  // allow local respawn
    // There is an issue with the activation and dea=ctivationg of the respawn. It require longer time for response.
    // Will have to see how to decreate the response time
    // Maybe use bluetooth to send the signal to the weapon
    uint32_t respawnDelay = 7;  // How many seconds a player must wait before being able to respawn
    uint32_t spawnTime = 0;     // time stamp of last respawn;

    uint32_t deathTime = 0;     // time stamp of last death
    uint32_t deathTimeout = 5;  // delay before respawn is possible

    // Default constructor
    Player() = default;

    // Constructor
    Player(uint32_t _playerId, uint32_t _team, String _name, PLAYERSTATE _playerState, String _playerAddr, std::vector<String> _weaponAddrs, uint32_t _score, uint32_t _kills, uint32_t _deaths, uint32_t _cpoint, uint32_t _assists,
           uint32_t _maxHealth, uint32_t _minHealth, bool _localRespawn, uint32_t _respawnDelay)
        : playerId(_playerId),
          team(_team),
          name(_name),
          playerState(_playerState),
          playerAddr(_playerAddr),
          weaponAddrs(_weaponAddrs),
          score(_score),
          kills(_kills),
          deaths(_deaths),
          cpoint(_cpoint),
          assists(_assists),
          maxHealth(_maxHealth),
          minHealth(_minHealth),
          localRespawn(_localRespawn),
          respawnDelay(_respawnDelay) {}

    // Add a constructor to initialize the Player object
    Player(uint32_t _playerId, uint32_t _team, String _name, PLAYERSTATE _playerState, String _playerAddr) : playerId(_playerId), team(_team), name(_name), playerState(_playerState), playerAddr(_playerAddr) {}

    void setState(String stateName) {
        for (const auto &findPlayerState : PlayerStates) {
            if (findPlayerState.second == stateName) {
                playerState = findPlayerState.first;
                break;
            }
        }
    }

    String getState() const { return PlayerStates[playerState]; }

    int getHealth() {
        switch (playerState) {
            case PLAYERSTATE::RESPAWNING:
                return -1;
            case PLAYERSTATE::DEAD:
                return -4;
            default:
                return health;
        }
    }
};

/****************************************************************************************************************************************************************************************/

// struct GameSetup {
//     GAMEMODES gameMode = GAMEMODES::FREE_FOR_ALL;
//     GAMESTATE gameState = GAMESTATE::SETUP;
//     uint32_t gameStartTime = 0;
//     bool teamBased = true;
//     uint32_t teams = 2;
//     std::map<uint8_t, uint32_t> teamScore;
//     bool friendlyFire = false;
//     uint32_t playerCount = 0;
//     uint32_t playerId = 999;
//     std::map<uint32_t, Player> players = std::map<uint32_t, Player>();
//     LAUNCHTRIGGER launchTrigger = LAUNCHTRIGGER::TIMER;
//     uint32_t launchTime = 0;
//     STOPTRIGGER stopTrigger = STOPTRIGGER::SCORE;
//     uint32_t stopLimit = 100;
//     uint32_t killMultiplier = 10;
//     uint8_t killIncrement = 1;
//     uint32_t assistMultiplier = 5;
//     uint8_t assistIncrement = 1;
//     uint32_t gameModePoints = 0;
//     uint32_t conquestPoints = 0;
//     bool extendedUpdates = false;
//     // Used to create a pointer to the this player
//     game_management::Player *player;
//     // Default constructor
//     GameSetup() = default;

//     // Add a constructor to initialize the GameSetup object
//     GameSetup(GAMEMODES _gameMode, GAMESTATE _gameState, uint32_t _gameStartTime, bool _teamBased, uint32_t _teams, bool _friendlyFire, uint32_t _playerCount, std::map<uint32_t, Player> _players, LAUNCHTRIGGER _launchTrigger,
//               uint32_t _launchTime, STOPTRIGGER _stopTrigger, uint32_t _stopLimit, uint32_t _killMultiplier, uint32_t _assistMultiplier, uint32_t _gameModePoints, uint32_t _conquestPoints, bool _extendedUpdates)
//         : gameMode(_gameMode),
//           gameState(_gameState),
//           gameStartTime(_gameStartTime),
//           teamBased(_teamBased),
//           teams(_teams),
//           friendlyFire(_friendlyFire),
//           playerCount(_playerCount),
//           players(_players),
//           launchTrigger(_launchTrigger),
//           launchTime(_launchTime),
//           stopTrigger(_stopTrigger),
//           stopLimit(_stopLimit),
//           killMultiplier(_killMultiplier),
//           assistMultiplier(_assistMultiplier),
//           gameModePoints(_gameModePoints),
//           conquestPoints(_conquestPoints),
//           extendedUpdates(_extendedUpdates) {}

//     GameSetup(GAMEMODES _gameMode, GAMESTATE _gameState, bool _teamBased, uint32_t _teams, bool _friendlyFire, uint32_t _playerCount, std::map<uint32_t, Player> _players, LAUNCHTRIGGER _launchTrigger, uint32_t _launchTime,
//               STOPTRIGGER _stopTrigger, uint32_t _stopLimit)
//         : gameMode(GAMEMODES::FREE_FOR_ALL),
//           gameState(GAMESTATE::SETUP),
//           gameStartTime(0),
//           teamBased(true),
//           teams(2),
//           friendlyFire(false),
//           playerCount(0),
//           players({}),
//           launchTrigger(_launchTrigger),
//           launchTime(_launchTime),
//           stopTrigger(_stopTrigger),
//           stopLimit(600),
//           killMultiplier(10),
//           assistMultiplier(5),
//           gameModePoints(5),
//           conquestPoints(0),
//           extendedUpdates(true) {}
struct GameSetup {
    GAMEMODES gameMode = GAMEMODES::FREE_FOR_ALL;
    GAMESTATE gameState = GAMESTATE::SETUP;
    uint32_t gameStartTime = 0;
    bool teamBased = true;
    uint32_t teams = 2;
    std::map<uint8_t, uint32_t> teamScore{};
    bool friendlyFire = true;
    uint32_t playerCount = 0;
    uint32_t playerId = 999;
    std::map<uint32_t, Player> players{};
    LAUNCHTRIGGER launchTrigger = LAUNCHTRIGGER::TIMER;
    uint32_t launchTime = 0;
    STOPTRIGGER stopTrigger = STOPTRIGGER::SCORE;
    uint32_t stopLimit = 100;
    uint32_t killMultiplier = 10;
    uint8_t killIncrement = 1;
    uint32_t assistMultiplier = 5;
    uint8_t assistIncrement = 1;
    uint32_t gameModePoints = 0;
    uint32_t conquestPoints = 0;
    bool extendedUpdates = false;
    game_management::Player *player = nullptr;

    GameSetup()
        : gameMode(GAMEMODES::FREE_FOR_ALL),
          gameState(GAMESTATE::SETUP),
          gameStartTime(0),
          teamBased(true),
          teams(2),
          teamScore(),
          friendlyFire(true),
          playerCount(0),
          playerId(999),
          players(),
          launchTrigger(LAUNCHTRIGGER::TIMER),
          launchTime(0),
          stopTrigger(STOPTRIGGER::SCORE),
          stopLimit(100),
          killMultiplier(10),
          killIncrement(1),
          assistMultiplier(5),
          assistIncrement(1),
          gameModePoints(0),
          conquestPoints(0),
          extendedUpdates(false),
          player(nullptr) {}

    // Custom constructor
    GameSetup(GAMEMODES _gameMode, GAMESTATE _gameState, uint32_t _gameStartTime, bool _teamBased, uint32_t _teams, bool _friendlyFire, uint32_t _playerCount, const std::map<uint32_t, Player> &_players, LAUNCHTRIGGER _launchTrigger,
              uint32_t _launchTime, STOPTRIGGER _stopTrigger, uint32_t _stopLimit, uint32_t _killMultiplier, uint32_t _assistMultiplier, uint32_t _gameModePoints, uint32_t _conquestPoints, bool _extendedUpdates)
        : gameMode(_gameMode),
          gameState(_gameState),
          gameStartTime(_gameStartTime),
          teamBased(_teamBased),
          teams(_teams),
          teamScore(),
          friendlyFire(_friendlyFire),
          playerCount(_playerCount),
          playerId(999),
          players(_players),
          launchTrigger(_launchTrigger),
          launchTime(_launchTime),
          stopTrigger(_stopTrigger),
          stopLimit(_stopLimit),
          killMultiplier(_killMultiplier),
          killIncrement(1),
          assistMultiplier(_assistMultiplier),
          assistIncrement(1),
          gameModePoints(_gameModePoints),
          conquestPoints(_conquestPoints),
          extendedUpdates(_extendedUpdates),
          player(nullptr) {}

    // Reset function to reinitialize all members to their default values
    void reset() { *this = GameSetup(); }
    /****************************************************************************************************************************************************************************************/

    String getGameMode() { return GameModes[gameMode].first; }

    /****************************************************************************************************************************************************************************************/

    void setGameMode(GAMEMODES _gameMode) {
        gameMode = _gameMode;
        teamBased = GameModes[_gameMode].second;
    }

    /****************************************************************************************************************************************************************************************/

    void setGameMode(String modeName) {
        for (const auto &findGameMode : GameModes) {
            if (findGameMode.second.first == modeName) {
                gameMode = findGameMode.first;
                teamBased = findGameMode.second.second;
                break;
            }
        }
    }

    /****************************************************************************************************************************************************************************************/

    String getGameState() { return GameStates[gameState]; }

    void setGameState(String stateName) {
        for (const auto &findGameState : GameStates) {
            if (findGameState.second == stateName) {
                gameState = findGameState.first;
                break;
            }
        }
    }

    /****************************************************************************************************************************************************************************************/

    String getLaunchTrigger() { return LaunchTriggers[launchTrigger]; }

    /****************************************************************************************************************************************************************************************/

    void setLaunchTrigger(String triggerName) {
        for (const auto &findLaunchTrigger : LaunchTriggers) {
            if (findLaunchTrigger.second == triggerName) {
                launchTrigger = findLaunchTrigger.first;
                break;
            }
        }
    }

    /****************************************************************************************************************************************************************************************/

    bool isEnemy(int offenderID) {
        if (player && players.find(offenderID) != players.end()) {
            return player->team != players[offenderID].team;
        }
        // Handle the case where one or both players don't exist
        // Return a default value or throw an exception, depending on your requirements
        return true;  // Default value
    }

    /****************************************************************************************************************************************************************************************/

    void printPlayerStats() {
        // debug
        Serial.println("Player absolute score:\n id\tscore\tkills\tassists\tdeaths");

        for (const auto &playerPair : players) {
            const Player &player = playerPair.second;
            Serial.println(String(player.playerId) + "\t" + String(player.score) + "\t" + String(player.kills) + "\t" + String(player.assists) + "\t" + String(player.deaths));
        }
    }

    /****************************************************************************************************************************************************************************************/

    Player parsePlayerJson(String playerJson) {
        JsonDocument doc;
        deserializeJson(doc, playerJson);

        // Create a new Player object
        Player newPlayer;

        // Check if each field exists in the JSON before trying to access it
        if (doc.containsKey("playerId")) {
            newPlayer.playerId = doc["playerId"];

            if (doc.containsKey("team")) {
                newPlayer.team = doc["team"];
            }
            if (doc.containsKey("name")) {
                newPlayer.name = doc["name"].as<String>();
            }
            if (doc.containsKey("state")) {
                newPlayer.setState(doc["state"].as<String>());
            }
            if (doc.containsKey("playerAddr")) {
                newPlayer.playerAddr = doc["playerAddr"].as<String>();
            }
            if (doc.containsKey("weaponAddrs")) {
                JsonArray array = doc["weaponAddrs"].as<JsonArray>();
                for (JsonVariant v : array) {
                    newPlayer.weaponAddrs.push_back(v.as<String>());
                }
            }
            if (doc.containsKey("score")) {
                newPlayer.score = doc["score"];
            }
            if (doc.containsKey("kills")) {
                newPlayer.kills = doc["kills"];
            }
            if (doc.containsKey("deaths")) {
                newPlayer.deaths = doc["deaths"];
            }
            if (doc.containsKey("cpoint")) {
                newPlayer.cpoint = doc["cpoint"];
            }
            if (doc.containsKey("assists")) {
                newPlayer.assists = doc["assists"];
            }
            if (doc.containsKey("maxHealth")) {
                newPlayer.maxHealth = doc["maxHealth"];
            }
            if (doc.containsKey("minHealth")) {
                newPlayer.minHealth = doc["minHealth"];
            }
            if (doc.containsKey("health")) {
                newPlayer.health = doc["health"];
            }
            if (doc.containsKey("localRespawn")) {
                newPlayer.localRespawn = doc["localRespawn"];
            }
            if (doc.containsKey("respawnDelay")) {
                newPlayer.respawnDelay = doc["respawnDelay"];
            }
        } else {
            Serial.println("Player ID not found in JSON");
            newPlayer.playerId = 999;
        }

        return newPlayer;
    }

    PLAYERSTATE getPlayerState(uint32_t playerId) { return players[playerId].playerState; }

    /****************************************************************************************************************************************************************************************/

    void setPlayer(uint32_t playerId) {
        this->playerId = playerId;      // Set the player ID
        player = &(players[playerId]);  // Set the player pointer
    }

    /****************************************************************************************************************************************************************************************/

    void saveGameSetup() {
        Serial.println("Saving game setup");
        Preferences preferences;
        preferences.begin("gameSetup", false);

        preferences.putInt("gameMode", static_cast<int>(gameMode));
        preferences.putInt("gameState", static_cast<int>(gameState));
        preferences.putUInt("gameStartTime", gameStartTime);
        preferences.putBool("teamBased", teamBased);
        preferences.putUInt("teams", teams);
        preferences.putBool("friendlyFire", friendlyFire);
        preferences.putUInt("playerCount", playerCount);
        if (playerId != 999) {
            preferences.putUInt("playerId", playerId);
        }
        preferences.putInt("launchTrigger", static_cast<int>(launchTrigger));
        preferences.putUInt("launchTime", launchTime);
        preferences.putInt("stopTrigger", static_cast<int>(stopTrigger));
        preferences.putUInt("stopLimit", stopLimit);
        preferences.putUInt("killMultiplr", killMultiplier);
        preferences.putUInt("assistMultiplr", assistMultiplier);
        preferences.putUInt("gameModePoints", gameModePoints);
        preferences.putUInt("conquestPoints", conquestPoints);
        preferences.putBool("extendedUpd", extendedUpdates);

        // Save each player's data
        for (const auto &player : players) {
            String baseKey = "P" + String(player.first);
            Serial.println("Saving player " + baseKey);
            preferences.putUInt((baseKey + "playerId").c_str(), player.second.playerId);
            preferences.putUInt((baseKey + "team").c_str(), player.second.team);
            preferences.putString((baseKey + "name").c_str(), player.second.name.c_str());
            preferences.putInt((baseKey + "state").c_str(), static_cast<int>(player.second.playerState));
            preferences.putString((baseKey + "playAddr").c_str(), player.second.playerAddr.c_str());
            preferences.putUInt((baseKey + "score").c_str(), player.second.score);
            preferences.putUInt((baseKey + "kills").c_str(), player.second.kills);
            preferences.putUInt((baseKey + "deaths").c_str(), player.second.deaths);
            preferences.putUInt((baseKey + "cpoint").c_str(), player.second.cpoint);
            preferences.putUInt((baseKey + "assists").c_str(), player.second.assists);
            preferences.putInt((baseKey + "maxHealth").c_str(), player.second.maxHealth);
            preferences.putInt((baseKey + "minHealth").c_str(), player.second.minHealth);
            preferences.putBool((baseKey + "localspw").c_str(), player.second.localRespawn);
            preferences.putUInt((baseKey + "spwDelay").c_str(), player.second.respawnDelay);
            String weaponAddrsStr = "";
            for (String addr : player.second.weaponAddrs) {
                weaponAddrsStr += String(addr) + ",";
            }

            preferences.putString((baseKey + "WpnAddrs").c_str(), weaponAddrsStr.c_str());
            Serial.println("Save - Player ID: " + String(player.second.playerId) + " Player Address: " + String(player.second.playerAddr) + " Number of weapon Addresses: " + String(player.second.weaponAddrs.size()));
        }

        preferences.end();
        Serial.println("Game setup saved");
    }

    /****************************************************************************************************************************************************************************************/

    void loadGameSetup() {
        Serial.println("Loading game setup");
        Preferences preferences;
        preferences.begin("gameSetup", true);

        gameMode = static_cast<GAMEMODES>(preferences.getInt("gameMode"));
        gameState = static_cast<GAMESTATE>(preferences.getInt("gameState"));
        gameStartTime = preferences.getUInt("gameStartTime");
        teamBased = preferences.getBool("teamBased");
        teams = preferences.getUInt("teams");
        friendlyFire = preferences.getBool("friendlyFire");
        playerCount = preferences.getUInt("playerCount", 0);
        playerId = preferences.getUInt("playerId", 999);
        launchTrigger = static_cast<LAUNCHTRIGGER>(preferences.getInt("launchTrigger"));
        launchTime = preferences.getUInt("launchTime");
        stopTrigger = static_cast<STOPTRIGGER>(preferences.getInt("stopTrigger"));
        stopLimit = preferences.getUInt("stopLimit");
        killMultiplier = preferences.getUInt("killMultiplr");
        assistMultiplier = preferences.getUInt("assistMultiplr");
        gameModePoints = preferences.getUInt("gameModePoints");
        conquestPoints = preferences.getUInt("conquestPoints");
        extendedUpdates = preferences.getBool("extendedUpd");

        players = std::map<uint32_t, Player>();
        Serial.println("Number of players: " + String(playerCount));

        // Restoring players
        for (uint32_t i = 0; i < playerCount; ++i) {
            String playerKey = "P" + String(i);
            Player player;

            player.playerId = preferences.getUInt((playerKey + "playerId").c_str(), 0);
            player.team = preferences.getUInt((playerKey + "team").c_str(), 0);
            player.name = preferences.getString((playerKey + "name").c_str(), "");
            player.playerState = static_cast<PLAYERSTATE>(preferences.getInt((playerKey + "state").c_str(), 0));
            player.playerAddr = preferences.getString((playerKey + "playAddr").c_str(), "");
            player.score = preferences.getUInt((playerKey + "score").c_str(), 0);
            player.kills = preferences.getUInt((playerKey + "kills").c_str(), 0);
            player.deaths = preferences.getUInt((playerKey + "deaths").c_str(), 0);
            player.cpoint = preferences.getUInt((playerKey + "cpoint").c_str(), 0);
            player.assists = preferences.getUInt((playerKey + "assists").c_str(), 0);
            player.maxHealth = preferences.getInt((playerKey + "maxHealth").c_str(), 0);
            player.minHealth = preferences.getInt((playerKey + "minHealth").c_str(), 0);
            player.health = preferences.getInt((playerKey + "health").c_str(), 100);
            player.localRespawn = preferences.getBool((playerKey + "localSpw").c_str(), false);
            player.respawnDelay = preferences.getUInt((playerKey + "spwDelay").c_str(), 0);
            player.deathTime = preferences.getUInt((playerKey + "deathTime").c_str(), 0);
            player.spawnTime = preferences.getUInt((playerKey + "spwTime").c_str(), 0);
            player.deathTimeout = preferences.getUInt((playerKey + "deathTimeout").c_str(), 0);

            String weaponAddrsStr = preferences.getString((playerKey + "WpnAddrs").c_str(), "").c_str();
            Serial.println("Weapon addresses: " + weaponAddrsStr);
            std::vector<uint32_t> weaponAddrs;
            int pos = 0;
            while ((pos = weaponAddrsStr.indexOf(',')) >= 0) {
                String addrStr = weaponAddrsStr.substring(0, pos);
                String addr = addrStr;
                player.weaponAddrs.push_back(addr);
                Serial.println("Weapon address: " + String(addr));
                weaponAddrsStr.remove(0, pos + 1);
            }
            if (weaponAddrsStr.length() > 0) {
                String addr = weaponAddrsStr;
                player.weaponAddrs.push_back(addr);
                Serial.println("Weapon address: " + weaponAddrsStr);
            }
            Serial.println("Load - Player ID: " + String(player.playerId) + " Player Address: " + String(player.playerAddr) + " Number of weapon Addresses: " + String(player.weaponAddrs.size()));

            players[player.playerId] = player;
        }

        preferences.end();
        Serial.println("Game setup loaded");
    }

    /****************************************************************************************************************************************************************************************/

    void resetGameSetup() {
        Serial.println("Resetting game setup");
        Preferences preferences;
        preferences.begin("gameSetup", false);
        preferences.clear();
        preferences.end();
        Serial.println("Game setup reset");
    }
};

}  // namespace game_management
#endif

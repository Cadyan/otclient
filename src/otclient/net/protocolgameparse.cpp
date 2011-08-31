/*
 * Copyright (c) 2010-2011 OTClient <https://github.com/edubart/otclient>
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 */

#include "protocolgame.h"

#include <otclient/core/localplayer.h>
#include <otclient/core/thingstype.h>
#include <otclient/core/game.h>
#include <otclient/core/map.h>
#include <otclient/core/item.h>
#include <otclient/core/effect.h>
#include <framework/core/eventdispatcher.h>

void ProtocolGame::parseMessage(InputMessage& msg)
{
    while(!msg.eof()) {
        uint8 opt = msg.getU8();
        //dump << "protocol id:" << std::hex << (int)opt;

        switch(opt) {
            case Otc::GameServerInitGame:
                parsePlayerLogin(msg);
                break;
            case 0x0B:
                parseGMActions(msg);
                break;
            case Otc::GameServerLoginError:
                parseErrorMessage(msg);
                break;
            case Otc::GameServerLoginAdvice:
                parseFYIMessage(msg);
                break;
            case Otc::GameServerLoginWait:
                parseWaitList(msg);
                break;
            case Otc::GameServerPing:
                parsePing(msg);
                break;
            //case Otc::GameServerChallange:
            case Otc::GameServerDead:
                parseDeath(msg);
                break;
            case Otc::GameServerFullMap:
                parseMapDescription(msg);
                break;
            case Otc::GameServerMapTopRow:
                parseMoveNorth(msg);
                break;
            case Otc::GameServerMapRightRow:
                parseMoveEast(msg);
                break;
            case Otc::GameServerMapBottomRow:
                parseMoveSouth(msg);
                break;
            case Otc::GameServerMapLeftRow:
                parseMoveWest(msg);
                break;
            case Otc::GameServerTileData:
                parseUpdateTile(msg);
                break;
            case Otc::GameServerCreateOnMap:
                parseTileAddThing(msg);
                break;
            case Otc::GameServerChangeOnMap:
                parseTileTransformThing(msg);
                break;
            case Otc::GameServerDeleteOnMap:
                parseTileRemoveThing(msg);
                break;
            case Otc::GameServerMoveCreature:
                parseCreatureMove(msg);
                break;
            case Otc::GameServerOpenContainer:
                parseOpenContainer(msg);
                break;
            case Otc::GameServerCloseContainer:
                parseCloseContainer(msg);
                break;
            case Otc::GameServerCreateContainer:
                parseContainerAddItem(msg);
                break;
            case Otc::GameServerChangeInContainer:
                parseContainerUpdateItem(msg);
                break;
            case Otc::GameServerDeleteInContainer:
                parseContainerRemoveItem(msg);
                break;
            case Otc::GameServerSetInventory:
                parseAddInventoryItem(msg);
                break;
            case Otc::GameServerDeleteInventory:
                parseRemoveInventoryItem(msg);
                break;
            case Otc::GameServerNpcOffer:
                parseOpenShopWindow(msg);
                break;
            case Otc::GameServerPlayerGoods:
                parsePlayerCash(msg);
                break;
            case Otc::GameServerCloseNpcTrade:
                parseCloseShopWindow(msg);
                break;
            case Otc::GameServerOwnOffer:
                parseSafeTradeRequest(msg);
                break;
            case Otc::GameServerCounterOffer:
                parseSafeTradeRequest(msg);
                break;
            case Otc::GameServerCloseTrade:
                parseSafeTradeClose(msg);
                break;
            case Otc::GameServerAmbient:
                parseWorldLight(msg);
                break;
            case Otc::GameServerGraphicalEffect:
                parseMagicEffect(msg);
                break;
            case Otc::GameServerTextEffect:
                parseAnimatedText(msg);
                break;
            case Otc::GameServerMissleEffect:
                parseDistanceShot(msg);
                break;
            case Otc::GameServerMarkCreature:
                parseCreatureSquare(msg);
                break;
            //case Otc::GameServerTrappers
            case Otc::GameServerCreatureHealth:
                parseCreatureHealth(msg);
                break;
            case Otc::GameServerCreatureLight:
                parseCreatureLight(msg);
                break;
            case Otc::GameServerCreatureOutfit:
                parseCreatureOutfit(msg);
                break;
            case Otc::GameServerCreatureSpeed:
                parseCreatureSpeed(msg);
                break;
            case Otc::GameServerCreatureSkull:
                parseCreatureSkulls(msg);
                break;
            case Otc::GameServerCreatureParty:
                parseCreatureShields(msg);
                break;
            // case Otc::GameServerCreatureUnpass
            case Otc::GameServerEditText:
                parseItemTextWindow(msg);
                break;
            case Otc::GameServerEditList:
                parseHouseTextWindow(msg);
                break;
            case Otc::GameServerPlayerData:
                parsePlayerStats(msg);
                break;
            case Otc::GameServerPlayerSkills:
                parsePlayerSkills(msg);
                break;
            case Otc::GameServerPlayerState:
                parsePlayerIcons(msg);
                break;
            case Otc::GameServerClearTarget:
                parsePlayerCancelAttack(msg);
                break;
            //case Otc::GameServerSpellDelay:
            //case Otc::GameServerSpellGroupDelay:
            case Otc::GameServerTalk:
                parseCreatureSpeak(msg);
                break;
            case Otc::GameServerChannels:
                parseChannelList(msg);
                break;
            case Otc::GameServerOpenChannel:
                parseOpenChannel(msg);
                break;
            case Otc::GameServerPrivateChannel:
                parseOpenPrivatePlayerChat(msg);
                break;
            case Otc::GameServerRuleViolation:
            case Otc::GameServerRuleViolation1:
            case Otc::GameServerRuleViolation2:
            case Otc::GameServerRuleViolation3:
                parseOpenRuleViolation(msg);
                break;
            case Otc::GameServerOpenOwnChannel:
                parseCreatePrivateChannel(msg);
                break;
            case Otc::GameServerCloseChannel:
                parseClosePrivateChannel(msg);
                break;
            case Otc::GameServerMessage:
                parseTextMessage(msg);
                break;
            case Otc::GameServerSnapBack:
                parseCancelWalk(msg);
                break;
            //case Otc::GameServerWait:
            case Otc::GameServerTopFloor:
                parseFloorChangeUp(msg);
                break;
            case Otc::GameServerBottomFloor:
                parseFloorChangeDown(msg);
                break;
            case Otc::GameServerOutfit:
                parseOutfitWindow(msg);
                break;
            case Otc::GameServerBuddyData:
                parseVipState(msg);
                break;
            case Otc::GameServerBuddyLogin:
                parseVipLogin(msg);
                break;
            case Otc::GameServerBuddyLogout:
                parseVipLogout(msg);
                break;
            case Otc::GameServerTutorialHint:
                parseShowTutorial(msg);
                break;
            case Otc::GameServerAutomapFlag:
                parseAddMarker(msg);
                break;
            case Otc::GameServerQuestLog:
                parseQuestList(msg);
                break;
            case Otc::GameServerQuestLine:
                parseQuestPartList(msg);
                break;
            //case Otc::GameServerChannelEvent:
            //case Otc::GameServerObjectInfo:
            //case Otc::GameServerPlayerInventory:
            default:
                logError("UNKNOWN PACKET OPT BYTE: 0x", std::hex, opt);
                break;
        }
    }
}

void ProtocolGame::parsePlayerLogin(InputMessage& msg)
{
    int playerId = msg.getU32();
    int playerDrawSpeed = msg.getU16();
    int playerCanReportBugs = msg.getU8();

    m_localPlayer = LocalPlayerPtr(new LocalPlayer);
    m_localPlayer->setId(playerId);
    m_localPlayer->setDrawSpeed(playerDrawSpeed);
    m_localPlayer->setCanReportBugs(playerCanReportBugs);
}

void ProtocolGame::parseGMActions(InputMessage& msg)
{
    for(uint8 i = 0; i < 28; ++i)
        msg.getU8();
}

void ProtocolGame::parseErrorMessage(InputMessage& msg)
{
    std::string error = msg.getString();
    g_game.processLoginError(error);
}

void ProtocolGame::parseFYIMessage(InputMessage& msg)
{
    msg.getString(); // message
}

void ProtocolGame::parseWaitList(InputMessage& msg)
{
    msg.getString(); // message
    msg.getU8();// + 1 // time
}

void ProtocolGame::parsePing(InputMessage&)
{
    sendPing();
}

void ProtocolGame::parseDeath(InputMessage&)
{

}

void ProtocolGame::parseMapDescription(InputMessage& msg)
{
    Position pos = parsePosition(msg);
    g_map.setCentralPosition(pos);
    setMapDescription(msg, pos.x - 8, pos.y - 6, pos.z, 18, 14);
}

void ProtocolGame::parseMoveNorth(InputMessage& msg)
{
    Position pos = g_map.getCentralPosition();
    pos.y--;

    setMapDescription(msg, pos.x - 8, pos.y - 6, pos.z, 18, 1);
    g_map.setCentralPosition(pos);
}

void ProtocolGame::parseMoveEast(InputMessage& msg)
{
    Position pos = g_map.getCentralPosition();
    pos.x++;

    setMapDescription(msg, pos.x + 9, pos.y - 6, pos.z, 1, 14);
    g_map.setCentralPosition(pos);
}

void ProtocolGame::parseMoveSouth(InputMessage& msg)
{
    Position pos = g_map.getCentralPosition();
    pos.y++;

    setMapDescription(msg, pos.x - 8, pos.y + 7, pos.z, 18, 1);
    g_map.setCentralPosition(pos);
}

void ProtocolGame::parseMoveWest(InputMessage& msg)
{
    Position pos = g_map.getCentralPosition();
    pos.x--;

    setMapDescription(msg, pos.x - 8, pos.y - 6, pos.z, 1, 14);
    g_map.setCentralPosition(pos);
}

void ProtocolGame::parseUpdateTile(InputMessage& msg)
{
    logDebug("PARSE UPDATE TILE!");

    Position tilePos = parsePosition(msg);
    uint16 thingId = msg.getU16(true);
    if(thingId == 0xFF01) {
        msg.getU16();
        /*msg->AddByte(0);
        msg->AddByte(0xFF);*/
    }
    else  {
        setTileDescription(msg, tilePos);
        msg.getU16();
        /*msg->AddByte(0x01);
        msg->AddByte(0xFF);*/
    }
}

void ProtocolGame::parseTileAddThing(InputMessage& msg)
{
    Position pos = parsePosition(msg);
    uint8 stackpos = msg.getU8();

    ThingPtr thing = internalGetThing(msg);
    thing->setPosition(pos);

    g_map.addThing(thing, stackpos);
}

void ProtocolGame::parseTileTransformThing(InputMessage& msg)
{
    Position pos = parsePosition(msg);
    uint8 stackpos = msg.getU8();

    uint16 thingId = msg.getU16();
    if(thingId == 0x0061 || thingId == 0x0062 || thingId == 0x0063) {
        parseCreatureTurn(msg);
    }
    else {
        ThingPtr thing = internalGetItem(msg, thingId);
        thing->setPosition(pos);

        g_map.removeThing(pos, stackpos);
        g_map.addThing(thing, stackpos);
    }
}

void ProtocolGame::parseTileRemoveThing(InputMessage& msg)
{
    Position pos = parsePosition(msg);
    uint8 stackpos = msg.getU8();

    g_map.removeThing(pos, stackpos);
}

void ProtocolGame::parseCreatureMove(InputMessage& msg)
{
    Position oldPos = parsePosition(msg);
    uint8 oldStackpos = msg.getU8();
    Position newPos = parsePosition(msg);

    ThingPtr thing = g_map.getThing(oldPos, oldStackpos);
    if(thing) {
        CreaturePtr creature = thing->asCreature();
        if(creature)
            creature->walk(newPos);
    }
}

void ProtocolGame::parseOpenContainer(InputMessage& msg)
{
    msg.getU8(); // cid
    msg.getU16(); // itemid
    msg.getString(); // name
    msg.getU8(); // capacity
    msg.getU8(); // hasParent
    uint8 size = msg.getU8(); // size

    for(uint32 i = 0; i < size; i++)
        internalGetItem(msg, 0xFFFF);
}

void ProtocolGame::parseCloseContainer(InputMessage& msg)
{
    msg.getU8(); // cid
}

void ProtocolGame::parseContainerAddItem(InputMessage& msg)
{
    msg.getU8(); // cid
    internalGetItem(msg, 0xFFFF);
}

void ProtocolGame::parseContainerUpdateItem(InputMessage& msg)
{
    msg.getU8(); // cid
    msg.getU8(); // slot
    internalGetItem(msg, 0xFFFF);
}

void ProtocolGame::parseContainerRemoveItem(InputMessage& msg)
{
    msg.getU8(); // cid
    msg.getU8(); // slot
}

void ProtocolGame::parseAddInventoryItem(InputMessage& msg)
{
    msg.getU8(); // slot
    internalGetItem(msg, 0xFFFF);
}

void ProtocolGame::parseRemoveInventoryItem(InputMessage& msg)
{
    msg.getU8(); // slot
}

void ProtocolGame::parseOpenShopWindow(InputMessage& msg)
{
    uint8 listCount = msg.getU8();
    for(uint8 i = 0; i < listCount; ++i) {
        msg.getU16(); // item id
        msg.getU8(); // runecharges
        msg.getString(); // item name
        msg.getU32(); // weight
        msg.getU32(); // buy price
        msg.getU32(); // sell price
    }
}

void ProtocolGame::parsePlayerCash(InputMessage& msg)
{
    msg.getU32();
    uint8 size = msg.getU8();

    for(int i = 0; i < size; i++) {
        msg.getU16(); // itemid
        msg.getU8(); // runecharges
    }
}

void ProtocolGame::parseCloseShopWindow(InputMessage&)
{
}

void ProtocolGame::parseSafeTradeRequest(InputMessage& msg)
{
    msg.getString(); // name
    uint8 count = msg.getU8();

    for(uint8 i = 0; i < count; i++)
        internalGetItem(msg, 0xFFFF);
}

void ProtocolGame::parseSafeTradeClose(InputMessage&)
{
}

void ProtocolGame::parseWorldLight(InputMessage& msg)
{
    Light light;
    light.intensity = msg.getU8();
    light.color = msg.getU8();

    g_map.setLight(light);
}

void ProtocolGame::parseMagicEffect(InputMessage& msg)
{
    EffectPtr effect = EffectPtr(new Effect());
    effect->setPosition(parsePosition(msg));
    effect->setId(msg.getU8());

    g_map.addThing(effect);
}

void ProtocolGame::parseAnimatedText(InputMessage& msg)
{
    parsePosition(msg); // textPos
    msg.getU8(); // color
    msg.getString(); // text
}

void ProtocolGame::parseDistanceShot(InputMessage& msg)
{
    parsePosition(msg); // fromPos
    parsePosition(msg); // toPos
    msg.getU8(); // effect
}

void ProtocolGame::parseCreatureSquare(InputMessage& msg)
{
    msg.getU32(); // creatureId
    msg.getU8(); // color
}

void ProtocolGame::parseCreatureHealth(InputMessage& msg)
{
    uint32 id = msg.getU32();
    uint8 healthPercent = msg.getU8();

    CreaturePtr creature = g_map.getCreatureById(id);
    if(creature)
        creature->setHealthPercent(healthPercent);
}

void ProtocolGame::parseCreatureLight(InputMessage& msg)
{
    uint32 id = msg.getU32();

    Light light;
    light.intensity = msg.getU8();
    light.color = msg.getU8();

    CreaturePtr creature = g_map.getCreatureById(id);
    if(creature)
        creature->setLight(light);
}

void ProtocolGame::parseCreatureOutfit(InputMessage& msg)
{
    uint32 id = msg.getU32();
    Outfit outfit = internalGetOutfit(msg);

    CreaturePtr creature = g_map.getCreatureById(id);
    if(creature)
        creature->setOutfit(outfit);
}

void ProtocolGame::parseCreatureSpeed(InputMessage& msg)
{
    uint32 id = msg.getU32();
    uint16 speed = msg.getU16();

    CreaturePtr creature = g_map.getCreatureById(id);
    if(creature)
        creature->setSpeed(speed);
}

void ProtocolGame::parseCreatureSkulls(InputMessage& msg)
{
    uint32 id = msg.getU32();
    uint8 skull = msg.getU8();

    CreaturePtr creature = g_map.getCreatureById(id);
    if(creature)
        creature->setSkull(skull);
}

void ProtocolGame::parseCreatureShields(InputMessage& msg)
{
    uint32 id = msg.getU32();
    uint8 shield = msg.getU8();

    CreaturePtr creature = g_map.getCreatureById(id);
    if(creature)
        creature->setShield(shield);
}

void ProtocolGame::parseCreatureTurn(InputMessage& msg)
{
    uint32 id = msg.getU32();
    Otc::Direction direction = (Otc::Direction)msg.getU8();

    CreaturePtr creature = g_map.getCreatureById(id);
    if(creature)
        creature->setDirection(direction);
}

void ProtocolGame::parseItemTextWindow(InputMessage& msg)
{
    msg.getU32(); // windowId
    msg.getU16(); // itemid
    msg.getU16(); // max length
    msg.getString(); // text
    msg.getString(); // writter
    msg.getString(); // date
}

void ProtocolGame::parseHouseTextWindow(InputMessage& msg)
{
    msg.getU8(); // unknown
    msg.getU32(); // windowId
    msg.getString(); // text
}

void ProtocolGame::parsePlayerStats(InputMessage& msg)
{
    msg.getU16(); // health
    msg.getU16(); // max health
    msg.getU32(); // free capacity
    msg.getU32(); // experience
    msg.getU16(); // level
    msg.getU8(); // level percent
    msg.getU16(); // mana
    msg.getU16(); // max mana
    msg.getU8(); // magic level
    msg.getU8(); // magic level percent
    msg.getU8(); // soul
    msg.getU16(); // stamina
}

void ProtocolGame::parsePlayerSkills(InputMessage& msg)
{
    for(int skill = 0; skill < Otc::LastSkill; skill++)
        for(int skillType = 0; skillType < Otc::LastSkillType; skillType++)
            m_localPlayer->setSkill((Otc::Skill)skill, (Otc::SkillType)skillType, msg.getU8());
}

void ProtocolGame::parsePlayerIcons(InputMessage& msg)
{
    msg.getU16(); // icons
}

void ProtocolGame::parsePlayerCancelAttack(InputMessage& msg)
{
    msg.getU32();
}

void ProtocolGame::parseCreatureSpeak(InputMessage& msg)
{
    msg.getU32(); // unkSpeak
    msg.getString(); // name
    msg.getU16(); // level
    uint8 type = msg.getU8();

    switch(type) {
        case Otc::SpeakSay:
        case Otc::SpeakWhisper:
        case Otc::SpeakYell:
        case Otc::SpeakMonsterSay:
        case Otc::SpeakMonsterYell:
        case Otc::SpeakPrivateNpcToPlayer:
            parsePosition(msg); // creaturePos
            break;
        case Otc::SpeakChannelRed:
        case Otc::SpeakChannelOrange:
        case Otc::SpeakChannelYellow:
        case Otc::SpeakChannelWhite:
            msg.getU16(); // channelId
            break;
        case Otc::SpeakPrivate:
        case Otc::SpeakPrivatePlayerToNpc:
        case Otc::SpeakBroadcast:
        case Otc::SpeakPrivateRed:
            break;
        default:
            logTraceDebug("Unknown speak type.", (int)type);
            break;
    }

    msg.getString(); // message
}

void ProtocolGame::parseChannelList(InputMessage& msg)
{
    uint8 count = msg.getU8();
    for(uint8 i = 0; i < count; i++) {
        msg.getU16();
        msg.getString();
    }
}

void ProtocolGame::parseOpenChannel(InputMessage& msg)
{
    msg.getU16(); // channelId
    msg.getString(); // name
}

void ProtocolGame::parseOpenPrivatePlayerChat(InputMessage& msg)
{
    msg.getString(); // name
}

void ProtocolGame::parseOpenRuleViolation(InputMessage& msg)
{
    msg.getU16(); // a
}

void ProtocolGame::parseCreatePrivateChannel(InputMessage& msg)
{
    msg.getU16(); // channel id
    msg.getString(); // channel name
}

void ProtocolGame::parseClosePrivateChannel(InputMessage& msg)
{
    msg.getU16(); // channel id
}

void ProtocolGame::parseTextMessage(InputMessage& msg)
{
    msg.getU8(); // messageType
    std::string message = msg.getString();

    // must be scheduled because the map may not exist yet
    g_dispatcher.addEvent(std::bind(&Game::processTextMessage, &g_game, message));
}

void ProtocolGame::parseCancelWalk(InputMessage& msg)
{
    Otc::Direction direction = (Otc::Direction)msg.getU8();
    m_localPlayer->cancelWalk(direction);
}

void ProtocolGame::parseFloorChangeUp(InputMessage& msg)
{
    logDebug("[ProtocolGame::parseFloorChangeUp]: This function has never been tested.");

    Position pos = g_map.getCentralPosition();
    pos.z--;

    int32 skip = 0;
    if(pos.z == 7)
        for(int32 i = 5; i >= 0; i--)
            setFloorDescription(msg, pos.x - 8, pos.y - 6, i, 18, 14, 8 - i, &skip);
    else if(pos.z > 7)
        setFloorDescription(msg, pos.x - 8, pos.y - 6, pos.z - 2, 18, 14, 3, &skip);

    pos.x++;
    pos.y++;
    g_map.setCentralPosition(pos);
}

void ProtocolGame::parseFloorChangeDown(InputMessage& msg)
{
    logDebug("[ProtocolGame::parseFloorChangeDown]: This function has never been tested.");

    Position pos = g_map.getCentralPosition();
    pos.z++;

    int skip = 0;
    if(pos.z == 8) {
        int j, i;
        for(i = pos.z, j = -1; i < pos.z + 3; ++i, --j)
            setFloorDescription(msg, pos.x - 8, pos.y - 6, i, 18, 14, j, &skip);
    }
    else if(pos.z > 8 && pos.z < 14)
        setFloorDescription(msg, pos.x - 8, pos.y - 6, pos.z + 2, 18, 14, -3, &skip);

    pos.x--;
    pos.y--;
    g_map.setCentralPosition(pos);
}

void ProtocolGame::parseOutfitWindow(InputMessage& msg)
{
    internalGetOutfit(msg);
    uint8 outfitCount = msg.getU8();

    for(int i = 0; i < outfitCount; i++) {
        msg.getU16(); // outfit id
        msg.getString(); // outfit name
        msg.getU8(); // addons
    }
}

void ProtocolGame::parseVipState(InputMessage& msg)
{
    msg.getU32(); // player id
    msg.getString(); // player name
    msg.getU8(); // online
}

void ProtocolGame::parseVipLogin(InputMessage& msg)
{
    msg.getU32(); // player id
}

void ProtocolGame::parseVipLogout(InputMessage& msg)
{
    msg.getU32(); // player id
}

void ProtocolGame::parseShowTutorial(InputMessage& msg)
{
    msg.getU8(); // tutorial id
}

void ProtocolGame::parseAddMarker(InputMessage& msg)
{
    parsePosition(msg); // position
    msg.getU8(); // icon
    msg.getString(); // message
}

void ProtocolGame::parseQuestList(InputMessage& msg)
{
    uint16 questsCount = msg.getU16();
    for(uint16 i = 0; i < questsCount; i++) {
        msg.getU16(); // quest id
        msg.getString(); // quest name
        msg.getU8(); // quest state
    }
}

void ProtocolGame::parseQuestPartList(InputMessage& msg)
{
    msg.getU16(); // quest id
    uint8 missionCount = msg.getU8();
    for(uint8 i = 0; i < missionCount; i++) {
        msg.getString(); // quest name
        msg.getString(); // quest description
    }
}

void ProtocolGame::setMapDescription(InputMessage& msg, int32 x, int32 y, int32 z, int32 width, int32 height)
{
    int startz, endz, zstep, skip = 0;

    if(z > 7) {
        startz = z - 2;
        endz = (15 < z+2) ? 15 : z+2;
        zstep = 1;
    }
    else {
        startz = 7;
        endz = 0;
        zstep = -1;
    }

    for(int nz = startz; nz != endz + zstep; nz += zstep)
        setFloorDescription(msg, x, y, nz, width, height, z - nz, &skip);
}

void ProtocolGame::setFloorDescription(InputMessage& msg, int32 x, int32 y, int32 z, int32 width, int32 height, int32 offset, int32* skipTiles)
{
    int32 skip = *skipTiles;

    for(int32 nx = 0; nx < width; nx++) {
        for(int32 ny = 0; ny < height; ny++) {
            if(skip == 0) {
                uint16 tileOpt = msg.getU16(true);
                if(tileOpt >= 0xFF00)
                    skip = (msg.getU16() & 0xFF);
                else {
                    Position pos(x + nx + offset, y + ny + offset, z);
                    setTileDescription(msg, pos);
                    skip = (msg.getU16() & 0xFF);
                }
            }
            else
                skip--;
        }
    }
    *skipTiles = skip;
}

void ProtocolGame::setTileDescription(InputMessage& msg, Position position)
{
    g_map.cleanTile(position);

    int stackpos = 0;
    while(1){
        uint16 inspectTileId = msg.getU16(true);
        if(inspectTileId >= 0xFF00)
            return;
        else {
            if(stackpos >= 10) {
                logTraceDebug("Too many things!.");
                return;
            }

            ThingPtr thing = internalGetThing(msg);
            if(thing)
                thing->setPosition(position);
            g_map.addThing(thing);
        }
        stackpos++;
    }
}

Outfit ProtocolGame::internalGetOutfit(InputMessage& msg)
{
    Outfit outfit;

    outfit.type = msg.getU16(); // looktype
    if(outfit.type != 0) {
        outfit.head = msg.getU8();
        outfit.body = msg.getU8();
        outfit.legs = msg.getU8();
        outfit.feet = msg.getU8();
        outfit.addons = msg.getU8();
    }
    else {
        outfit.type = msg.getU16();
    }

    return outfit;
}

ThingPtr ProtocolGame::internalGetThing(InputMessage& msg)
{
    ThingPtr thing;

    uint16 thingId = msg.getU16();
    if(thingId == 0x0061 || thingId == 0x0062) { // add new creature
        CreaturePtr creature = CreaturePtr(new Creature);

        if(thingId == 0x0062) { //creature is known
            uint32 id = msg.getU32();

            CreaturePtr knownCreature = g_map.getCreatureById(id);
            if(knownCreature)
                creature = knownCreature;
        }
        else if(thingId == 0x0061) { //creature is not known
            uint32 removeId = msg.getU32();
            uint32 id = msg.getU32();
            std::string name = msg.getString();

            g_map.removeCreatureById(removeId);

            if(m_localPlayer->getId() == id)
                creature = m_localPlayer->asCreature();

            creature->setId(id);
            creature->setName(name);
        }

        uint8 healthPercent = msg.getU8();
        Otc::Direction direction = (Otc::Direction)msg.getU8();
        Outfit outfit = internalGetOutfit(msg);

        Light light;
        light.intensity = msg.getU8();
        light.color = msg.getU8();

        uint16 speed = msg.getU16();
        uint8 skull = msg.getU8();
        uint8 shield = msg.getU8();

        uint8 emblem = 0;
        if(thingId == 0x0061) // emblem is sent only in packet type 0x61
            emblem = msg.getU8();

        bool impassable = (msg.getU8() == 0); // impassable

        creature->setHealthPercent(healthPercent);
        creature->setDirection(direction);
        creature->setOutfit(outfit);
        creature->setLight(light);
        creature->setSpeed(speed);
        creature->setSkull(skull);
        creature->setShield(shield);
        creature->setEmblem(emblem);
        creature->setImpassable(impassable);
        creature->cancelWalk(direction);

        thing = creature;

        // login event is generated the first time that local player gets known
        if(!g_game.isOnline() && creature == m_localPlayer) {
            // this event must be scheduled because the entire map description is not known yet
            g_dispatcher.addEvent(std::bind(&Game::processLogin, &g_game, m_localPlayer));
        }
    }
    else if(thingId == 0x0063) { // creature turn
        parseCreatureTurn(msg);
    }
    else // item
        thing = internalGetItem(msg, thingId);

    return thing;
}

ItemPtr ProtocolGame::internalGetItem(InputMessage& msg, uint16 id)
{
    ItemPtr item = ItemPtr(new Item());

    if(id == 0xFFFF)
        id = msg.getU16();
    item->setId(id);

    const ThingType& itemType = g_thingsType.getItemType(id);
    if(itemType.isStackable || itemType.isFluidContainer || itemType.isSplash)
        item->setCount(msg.getU8());

    return item;
}

Position ProtocolGame::parsePosition(InputMessage& msg)
{
    uint16 x = msg.getU16();
    uint16 y = msg.getU16();
    uint8  z = msg.getU8();

    return Position(x, y, z);
}

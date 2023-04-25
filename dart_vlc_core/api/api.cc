// This file is a part of dart_vlc (https://github.com/alexmercerind/dart_vlc)
//
// Copyright (C) 2021-2022 Hitesh Kumar Saini <saini123hitesh@gmail.com>
//
// This program is free software; you can redistribute it and/or
// modify it under the terms of the GNU Lesser General Public
// License as published by the Free Software Foundation; either
// version 3 of the License, or (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
// Lesser General Public License for more details.
//
// You should have received a copy of the GNU Lesser General Public License
// along with this program; if not, write to the Free Software Foundation,
// Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.

#include "api/api.h"
#include "event_manager.h"
#include "core.h"

namespace DartObjects {

struct DeviceList {
  // The device list that gets exposed to Dart.
  DartDeviceList dart_object;

  // Previousing data.
  std::vector<DartDeviceList::Device> device_infos;
  std::vector<Device> devices;
};

struct Equalizer {
  // The equalizer that gets exposed to Dart.
  DartEqualizer dart_object;

  // Previousing data.
  std::vector<float> bands;
  std::vector<float> amps;
};

static void DestroyObject(void*, void* peer) { delete peer; }

}  // namespace DartObjects

#ifdef __cplusplus
extern "C" {
#endif

std::map<int32_t, EventCallbackStruct*> _playerCallbackMap;
std::map<int32_t, bool> _playerCallbackHasSetted;

void _SetPlayerCallback(int32_t id) {
    auto player = g_players->Get(id);
    if (!player) {
        return;
    }
    if (_playerCallbackHasSetted[id] == true) {
        return;
    }
    _playerCallbackHasSetted[id] = true;
    player->SetPlayCallback(
        [=]() -> void {
            if (_playerCallbackMap[id] == nullptr)return;
            auto callback = _playerCallbackMap[id]->onPlayPauseStop;
            if (callback) {
                callback(id, player->state());
            }
        }
    );
    player->SetPauseCallback(
        [=]() -> void { 
            if (_playerCallbackMap[id] == nullptr)return;
            auto callback = _playerCallbackMap[id]->onPlayPauseStop;
            if (callback) {
                callback(id, player->state());
            }
        }
    );
    player->SetStopCallback([=]() -> void {
        if (_playerCallbackMap[id] == nullptr)return;
        auto callback = _playerCallbackMap[id]->onPlayPauseStop;
        if (callback) {
            callback(id, player->state());
        }
        callback = _playerCallbackMap[id]->onPosition;
        if (callback) {
            callback(id, player->state());
        }
    });
    player->SetCompleteCallback(
        [=]() -> void { 
            if (_playerCallbackMap[id] == nullptr)return;
            auto callback = _playerCallbackMap[id]->onComplete;
            if (callback) {
                callback(id, player->state());
            }
        }
    );
    player->SetVolumeCallback(
        [=](float) -> void { 
            if (_playerCallbackMap[id] == nullptr)return;
            auto callback = _playerCallbackMap[id]->onVolume;
            if (callback) {
                callback(id, player->state());
            }
        }
    );
    player->SetRateCallback(
        [=](float) -> void { 
            if (_playerCallbackMap[id] == nullptr)return;
            auto callback = _playerCallbackMap[id]->onRate;
            if (callback) {
                callback(id, player->state());
            }
        }
    );
    player->SetPositionCallback(
        [=](int32_t) -> void {
            if (_playerCallbackMap[id] == nullptr)return;
            auto callback = _playerCallbackMap[id]->onPosition;
            if (callback) {
                callback(id, player->state());
            }
        }
    );
    player->SetOpenCallback(
        [=](VLC::Media) -> void { 
            if (_playerCallbackMap[id] == nullptr)return;
            auto callback = _playerCallbackMap[id]->onOpen;
            if (callback) {
                callback(id, player->state());
            }
        }
    );
    player->SetPlaylistCallback(
        [=]() -> void { 
            if (_playerCallbackMap[id] == nullptr)return;
            auto callback = _playerCallbackMap[id]->onOpen;
            if (callback) {
                callback(id, player->state());
            }
        }
    );
    player->SetBufferingCallback(
        [=](float buffering) -> void { 
            if (_playerCallbackMap[id] == nullptr)return;
            auto callback = _playerCallbackMap[id]->onBuffering;
            if (callback) {
                callback(id, buffering);
            }
        }
    );
    player->SetVideoDimensionsCallback(
        [=](int32_t video_width, int32_t video_height) -> void {
            if (_playerCallbackMap[id] == nullptr)return;
            auto callback = _playerCallbackMap[id]->onVideoDimensions;
            if (callback) {
                callback(id, video_width, video_height);
            }
        }
    );
    player->SetVideoFrameCallback(
        [=](uint8_t* frame, int32_t width, int32_t height)->void {
            if (_playerCallbackMap[id] == nullptr)return;
            auto callback = _playerCallbackMap[id]->onVideoFrame;
            if (callback) {
                callback(id,frame,width,height);
            }
        }
    );
    player->SetErrorCallback(
        [=](std::string error) -> void { 
            if (_playerCallbackMap[id] == nullptr)return;
            auto callback = _playerCallbackMap[id]->onError;
            if (callback) {
                callback(id, error.c_str());
            }
        }
    );
}

void SetPlayerCallback(int32_t id, EventCallbackStruct& callbackObj)
{
    _playerCallbackMap.insert(std::make_pair(id, &callbackObj));
    _SetPlayerCallback(id);
}

Player* _GetPlayerById(int32_t id) {
    auto player = g_players->Get(id);
    if (!player) {
        g_players->Create(
            id, std::move(std::make_unique<Player>(std::vector<std::string>{})));
        player = g_players->Get(id);
    }

    if (_playerCallbackMap.find(id)!= _playerCallbackMap.end()) {
        _SetPlayerCallback(id);
    }

    return player;
}

void PlayerCreate(int32_t id, int32_t video_width, int32_t video_height,
                  int32_t argc, const char** argv, EventCallbackStruct& callbackObj) {

  
  std::vector<std::string> args{};
  for (int32_t i = 0; i < argc; i++) {
    args.emplace_back(argv[i]);
  }
  auto player = _GetPlayerById(id);
  if (video_width > 0 && video_height > 0) {
    player->SetVideoWidth(video_width);
    player->SetVideoHeight(video_height);
  }
  SetPlayerCallback(id,callbackObj);
}

void PlayerDispose(int32_t id) { 
    g_players->Dispose(id);
    _playerCallbackMap.erase(id);
    _playerCallbackHasSetted.erase(id);
}

void PlayerOpen(int32_t id, bool auto_start, const char** source,
                int32_t source_size) {
  std::vector<std::shared_ptr<Media>> medias{};
  auto player = _GetPlayerById(id);
  for (int32_t index = 0; index < 4 * source_size; index += 4) {
    std::shared_ptr<Media> media;
    const char* type = source[index];
    const char* resource = source[index + 1];
    const char* start_time = source[index + 2];
    const char* stop_time = source[index + 3];
    if (strcmp(type, "MediaType.file") == 0)
      media = Media::File(resource, false, 10000, start_time, stop_time);
    else if (strcmp(type, "MediaType.network") == 0)
      media = Media::Network(resource, false, 10000, start_time, stop_time);
    else
      media = Media::DirectShow(resource);
    medias.emplace_back(media);
  }
  player->Open(std::make_shared<Playlist>(medias), auto_start);
}

void PlayerPlay(int32_t id) {
  auto player = _GetPlayerById(id);
  player->Play();
}

void PlayerPause(int32_t id) {
  auto player = _GetPlayerById(id);
  player->Pause();
}

void PlayerPlayOrPause(int32_t id) {
  auto player = _GetPlayerById(id);
  player->PlayOrPause();
}

void PlayerStop(int32_t id) {
  auto player = _GetPlayerById(id);
  player->Stop();
}

void PlayerNext(int32_t id) {
  auto player = _GetPlayerById(id);
  player->Next();
}

void PlayerPrevious(int32_t id) {
  auto player = _GetPlayerById(id);
  player->Previous();
}

void PlayerJumpToIndex(int32_t id, int32_t index) {
  auto player = _GetPlayerById(id);
  player->JumpToIndex(index);
}

void PlayerSeek(int32_t id, int32_t position) {
  auto player = _GetPlayerById(id);
  player->Seek(position);
}

void PlayerSetVolume(int32_t id, float volume) {
  auto player = _GetPlayerById(id);
  player->SetVolume(volume);
}

void PlayerSetRate(int32_t id, float rate) {
  auto player = _GetPlayerById(id);
  player->SetRate(rate);
}

void PlayerSetUserAgent(int32_t id, const char* userAgent) {
  auto player = _GetPlayerById(id);
  player->SetUserAgent(userAgent);
}

void PlayerSetDevice(int32_t id, const char* device_id,
                     const char* device_name) {
  auto player = _GetPlayerById(id);
  Device device(device_id, device_name);
  player->SetDevice(device);
}

void PlayerSetEqualizer(int32_t id, int32_t equalizer_id) {
  auto player = _GetPlayerById(id);
  Equalizer* equalizer = g_equalizers->Get(equalizer_id);
  player->SetEqualizer(equalizer);
}

void PlayerSetPlaylistMode(int32_t id, const char* mode) {
  auto player = _GetPlayerById(id);
  PlaylistMode playlistMode;
  if (strcmp(mode, "PlaylistMode.repeat") == 0)
    playlistMode = PlaylistMode::repeat;
  else if (strcmp(mode, "PlaylistMode.loop") == 0)
    playlistMode = PlaylistMode::loop;
  else
    playlistMode = PlaylistMode::single;
  player->SetPlaylistMode(playlistMode);
}

void PlayerAdd(int32_t id, const char* type, const char* resource) {
  auto player = _GetPlayerById(id);
  std::shared_ptr<Media> media;
  if (strcmp(type, "MediaType.file") == 0)
    media = Media::File(resource, false);
  else if (strcmp(type, "MediaType.network") == 0)
    media = Media::Network(resource, false);
  else
    media = Media::DirectShow(resource);
  player->Add(media);
}

void PlayerRemove(int32_t id, int32_t index) {
  auto player = _GetPlayerById(id);
  player->Remove(index);
}

void PlayerInsert(int32_t id, int32_t index, const char* type,
                  const char* resource) {
  auto player = _GetPlayerById(id);
  std::shared_ptr<Media> media;
  if (strcmp(type, "MediaType.file") == 0)
    media = Media::File(resource, false);
  else if (strcmp(type, "MediaType.network") == 0)
    media = Media::Network(resource, false);
  else
    media = Media::DirectShow(resource);
  player->Insert(index, media);
}

void PlayerMove(int32_t id, int32_t initial_index, int32_t final_index) {
  auto player = _GetPlayerById(id);
  player->Move(initial_index, final_index);
}

void PlayerTakeSnapshot(int32_t id, const char* file_path, int32_t width,
                        int32_t height) {
  auto player = _GetPlayerById(id);
  player->TakeSnapshot(file_path, width, height);
}

void PlayerSetAudioTrack(int32_t id, int32_t track) {
  auto player = _GetPlayerById(id);
  player->SetAudioTrack(track);
}

int32_t PlayerGetAudioTrackCount(int32_t id) {
  auto player = _GetPlayerById(id);
  return player->GetAudioTrackCount();
}

void PlayerSetHWND(int32_t id, int64_t hwnd) {
  auto player = _GetPlayerById(id);
  player->SetHWND(hwnd);
}

void MediaClearMap(void*, void* peer) {
  delete reinterpret_cast<std::map<std::string, std::string>*>(peer);
}

void MediaClearVector(void*, void* peer) {
  delete reinterpret_cast<std::vector<const char*>*>(peer);
}

const char** MediaParse(const char* type,
                        const char* resource, int32_t timeout) {
  std::shared_ptr<Media> media = Media::Create(type, resource, true, timeout);
  static std::map<std::string, std::string>* metas = new std::map<std::string, std::string>();
  metas->clear();
  for (auto iter = media->metas().begin(); iter != media->metas().end(); iter++) {
      metas->insert(std::make_pair(iter->first, iter->second));
  }
  static std::vector<const char*>* values = new std::vector<const char*>();
  values->clear();
  /*Dart_NewFinalizableHandle_DL(
      object, reinterpret_cast<void*>(metas), sizeof(metas),
      static_cast<Dart_HandleFinalizer>(MediaClearMap));
  Dart_NewFinalizableHandle_DL(
      object, reinterpret_cast<void*>(values), sizeof(values),
      static_cast<Dart_HandleFinalizer>(MediaClearVector));*/
  for (const auto& [key, value] : *metas) {
    values->emplace_back(value.c_str());
  }

  return values->data();
}

void BroadcastCreate(int32_t id, const char* type, const char* resource,
                     const char* access, const char* mux, const char* dst,
                     const char* vcodec, int32_t vb, const char* acodec,
                     int32_t ab) {
  std::shared_ptr<Media> media = Media::Create(type, resource);
  std::unique_ptr<BroadcastConfiguration> configuration =
      std::make_unique<BroadcastConfiguration>(access, mux, dst, vcodec, vb,
                                               acodec, ab);
  if (!g_broadcasts->Get(id)) {
    g_broadcasts->Create(id, std::make_unique<Broadcast>(
                                 std::move(media), std::move(configuration)));
  }
}

void BroadcastStart(int32_t id) {
  auto broadcast = g_broadcasts->Get(id);
  if (!broadcast) {
    g_broadcasts->Create(id, std::make_unique<Broadcast>(nullptr, nullptr));
    broadcast = g_broadcasts->Get(id);
  }
  broadcast->Start();
}

void BroadcastDispose(int32_t id) { g_broadcasts->Dispose(id); }

void ChromecastCreate(int32_t id, const char* type, const char* resource,
                      const char* ip_address) {
  std::shared_ptr<Media> media = Media::Create(type, resource);
  auto chromecast = g_chromecasts->Get(id);
  if (!chromecast) {
    g_chromecasts->Create(
        id, std::make_unique<Chromecast>(std::move(media), ip_address));
  }
}

void ChromecastStart(int32_t id) {
  auto chromecast = g_chromecasts->Get(id);
  if (!chromecast) {
    g_chromecasts->Create(id, std::make_unique<Chromecast>(nullptr, ""));
    chromecast = g_chromecasts->Get(id);
  }
  chromecast->Start();
}

void ChromecastDispose(int32_t id) { g_chromecasts->Dispose(id); }

void RecordCreate(int32_t id, const char* saving_file, const char* type,
                  const char* resource) {
  std::shared_ptr<Media> media = Media::Create(type, resource);
  auto record = g_records->Get(id);
  if (!record) {
    g_records->Create(id, std::make_unique<Record>(media, saving_file));
    record = g_records->Get(id);
  }
}

void RecordStart(int32_t id) {
  auto record = g_records->Get(id);
  if (!record) {
    g_records->Create(id, std::make_unique<Record>(nullptr, ""));
    record = g_records->Get(id);
  }
  record->Start();
}

void RecordDispose(int32_t id) { g_records->Dispose(id); }

DartDeviceList* DevicesAll() {
  static DartObjects::DeviceList* wrapper = new DartObjects::DeviceList();
  wrapper->devices = Devices::All();
  wrapper->device_infos.clear();

  for (const auto& device : wrapper->devices) {
    wrapper->device_infos.emplace_back(device.name().c_str(),
                                       device.id().c_str());
  }

  wrapper->dart_object.size = wrapper->device_infos.size();
  wrapper->dart_object.device_infos = wrapper->device_infos.data();

  /*Dart_NewFinalizableHandle_DL(
      object, wrapper, sizeof(*wrapper),
      static_cast<Dart_HandleFinalizer>(DartObjects::DestroyObject));*/
  return &wrapper->dart_object;
}

static DartEqualizer* EqualizerToDart(const Equalizer* equalizer, int32_t id) {
  auto wrapper = new DartObjects::Equalizer();
  for (const auto& [band, amp] : equalizer->band_amps()) {
    wrapper->bands.emplace_back(band);
    wrapper->amps.emplace_back(amp);
  }

  wrapper->dart_object.id = id;
  wrapper->dart_object.pre_amp = equalizer->pre_amp();
  wrapper->dart_object.bands = wrapper->bands.data();
  wrapper->dart_object.amps = wrapper->amps.data();
  wrapper->dart_object.size = wrapper->amps.size();

  /*Dart_NewFinalizableHandle_DL(
      dart_handle, wrapper, sizeof(*wrapper),
      static_cast<Dart_HandleFinalizer>(DartObjects::DestroyObject));*/

  return &wrapper->dart_object;
}

struct DartEqualizer* EqualizerCreateEmpty() {
  auto id = g_equalizers->Count();
  g_equalizers->Create(id, std::make_unique<Equalizer>());
  return EqualizerToDart(g_equalizers->Get(id), id);
}

struct DartEqualizer* EqualizerCreateMode(int32_t mode) {
  auto id = g_equalizers->Count();
  g_equalizers->Create(
      id, std::make_unique<Equalizer>(static_cast<EqualizerMode>(mode)));
  return EqualizerToDart(g_equalizers->Get(id), id);
}

void EqualizerSetBandAmp(int32_t id, float band, float amp) {
  g_equalizers->Get(id)->SetBandAmp(band, amp);
}

void EqualizerSetPreAmp(int32_t id, float amp) {
  g_equalizers->Get(id)->SetPreAmp(amp);
}

#ifdef __cplusplus
}
#endif
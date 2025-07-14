#pragma once

typedef struct __attribute__((packed)) {
  uint32_t magic;
  uint8_t major;
  uint8_t minor;
  uint8_t patch;
} _cmd_version_get_response_t;

static inline bool VersionCommand_Exec(CommandReport_t* command) {
  _cmd_version_get_response_t* response = (_cmd_version_get_response_t*) command->response.data;

  response->magic = 0x1337CAFE;
  response->major = 0;
  response->minor = 1;
  response->patch = 0;

  return true;
}
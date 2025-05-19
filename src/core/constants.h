#pragma once

namespace Constants {
  namespace MCCS {
    // MCCS Version 2.1 and 2.2
    namespace VCPCode {
      // Standard VCP codes
      // https://www.ddcutil.com/vcpinfo_output/
      enum std {
        BRIGHTNESS = 0x10,
        CONTRAST = 0x12,
      };
      // ddcutil detect
      namespace Manufacturer {
        namespace AcerXV272UV3 {
          enum Code {
            MODE = 0xE2,
          };
          // VCP = 0xE2 values
          enum ModeValue {
            USER = 0x00,
            STANDARD = 0x01,
            ECO = 0x02,
            GRAPHICS = 0x03,
            GAME_ACTION = 0x05,
            GAME_RACING = 0x06,
            GAME_SPORTS = 0x07,
            HDR = 0x0b,
          };
        } // namespace AcerXV272UV3
      }   // namespace Manufacturer
    }     // namespace VCPCode
  }       // namespace MCCS
  namespace Display {
    const short CONTINUOUS_FEATURE_MIN = 0;

    namespace Brightness {
      const short MAX = 100;
      const short DEFAULT = 50;
      const short STEP = 10;
    } // namespace Brightness

    namespace Contrast {
      const short MAX = 100;
      const short DEFAULT = 50;
      const short STEP = 5;
    } // namespace Contrast

    const short REFRESH_INTERVAL = 5000; // 5 seconds
  }                                      // namespace Display
} // namespace Constants
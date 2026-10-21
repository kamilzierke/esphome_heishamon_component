# Source attribution for 0.1.0-k1

Baseline: `12e659079a45265c780dcf1c377da206d747e5cb`.
Reviewed ElVit revision: `1ccbf6b86b2e57aee7bff0121f9c7af805e871c1`.
Protocol comparison: HeishaMon `7eebb5863369be8eb51ecac20585f8f9aa99d34e`.

The following changes from ElVit/esphome_components were selectively adapted:

| Source | Change | Revision |
|---|---|---|
| [PR #26](https://github.com/ElVit/esphome_components/pull/26) | Signed integer command encoding | `1e1ebbf711f3a8a31c233e1ec6a883fa3c809ff7` |
| [PR #27](https://github.com/ElVit/esphome_components/pull/27) | Receive task and queues | Merge `41b7588eeae974b88dc50ed4023a442f0293aa1c` |
| [PR #37](https://github.com/ElVit/esphome_components/pull/37) | High-pressure unit conversion | `410f81d8d2b18db1bc639e003350f82fbdd8ba2f` |
| [PR #38](https://github.com/ElVit/esphome_components/pull/38) | Climate state initialization | `d3fb826e31766dda9bc6f53b7b75e3e2c95e5180` |
| [PR #39](https://github.com/ElVit/esphome_components/pull/39) | UART configuration validation | `ef42876f7de5f44e036603bbddf6175b9661ead7` |

This is an adaptation, not a merge of the complete repository or a copy of release 0.0.10. The bounded parser, masked command transactions and gap-aware integrator address additional issues. The water_heater platform and unrelated maidesite_desk changes were not imported.

The [ElVit MIT license](LICENSE-ElVit.txt) retains the upstream copyright and terms. The new energy integrator is an independent implementation informed by [ESPHome total_daily_energy 2026.9.1](https://github.com/esphome/esphome/tree/2026.9.1/esphome/components/total_daily_energy); the [ESPHome license](LICENSE-ESPHome.txt) is included to document that source. These notices do not change the license of unrelated project files.

K-profile bounds reference Panasonic service manual PAPAMY2306062CE, pages 66, 136, 138 and 160: [service manual](https://www.taloon.com/media/attachments/PANKIT-WC05K6E5_PANWH-SDC0309K6E5_Huolto.pdf). The K-series diagrams on pages 73/81/85 and [HeishaMon issue #232](https://github.com/heishamon/HeishaMon/issues/232#issuecomment-972598352) support treating low pressure as unavailable. These references do not establish undocumented field encodings.

`tests/native/fixtures/k_standard.hex` and `k_extra.hex` are synthetic 203-byte protocol packets. Their payloads are constructed for specific parser/decoder assertions and are not full equipment-state examples. Packet headers, encoding rules and the model-recognition signature are protocol information; no installation capture is distributed with these fixtures.

#include "revolution/hbm/nw4hbm/ut/CharWriter.h"
#include "revolution/hbm/nw4hbm/ut/Color.h"

namespace nw4hbm {
namespace db {

// Dummy use for the Color destructor retained from this stripped translation
// unit. The linker discards the helper itself.
void dummy(ut::CharWriter* pCharWriter) {
    ut::Color color;
    pCharWriter->SetTextColor(color);
}

} // namespace db
} // namespace nw4hbm

#include "controller_handler.h"

namespace controller_network
{

types::Error LookupWaysideController(types::WaysideId &wayside, const types::TrackId track, const types::BlockId block)
{
    // TODO NNF-226
    wayside = 0;
    (void)(track); // UNUSED
    (void)(block); // UNUSED
    return types::Error::ERROR_NONE;
}

} // namespace controller_network
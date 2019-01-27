#pragma once

namespace network {
namespace ip {
namespace tcp {

class ConnectorBuilder final {
   public:
    Connector build();
};

}  // namespace tcp
}  // namespace ip
}  // namespace network

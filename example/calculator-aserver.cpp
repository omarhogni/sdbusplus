#include <net/poettering/Calculator/aserver.hpp>
#include <sdbusplus/async.hpp>

class Calculator :
    public sdbusplus::aserver::net::poettering::Calculator<Calculator>
{
  public:
    explicit Calculator(sdbusplus::async::context& ctx) :
        sdbusplus::aserver::net::poettering::Calculator<Calculator>(
            ctx, "/net/poettering/calculator"),
        manager(ctx, "/")
    {
        ctx.spawn(startup());
    }

    auto get_property(last_result_t) const
    {
        return _last_result;
    }

    bool set_property(last_result_t, int64_t v)
    {
        if (v % 2 == 0)
        {
            std::swap(_last_result, v);
            return v != _last_result;
        }
        return false;
    }

  private:
    auto startup() -> sdbusplus::async::task<>
    {
        last_result(123);
        ctx.get_bus().request_name("net.poettering.Calculator");

        status(State::Error);

        while (1)
        {
            using namespace std::literals;
            co_await sdbusplus::async::sleep_for(ctx, 10s);

            cleared(42);
        }
        co_return;
    }

    sdbusplus::server::manager_t manager;
};

int main()
{
    sdbusplus::async::context ctx;
    [[maybe_unused]] Calculator c(ctx);

    ctx.run();

    return 0;
}

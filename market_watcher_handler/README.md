<A name="toc2-1" title="Market Watcher: Handler" />
Market Watcher: Handler
----------------------
This example shows how to use the preferred method of subscribing to and receiving tick data: subclassing zenfire::feed::Handler.  If you've already written a zenfire::event::Handler for use with a zenfire::event::Client, this will be very familiar.  Tick data and status (up and down) are sent to handlers you write.

#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include <zmq.hpp>
#include <thread>
#include <chrono>

#include "mocks.h"
#include "../src/job/job_receiver.h"
#include "../src/eval_request.h"
#include "../src/connection_proxy.h"

using namespace testing;


TEST(job_receiver, basic)
{
	auto context = std::make_shared<zmq::context_t>(1);
	zmq::socket_t socket(*context, ZMQ_PAIR);
	socket.bind("inproc://" + JOB_SOCKET_ID);

	auto evaluator = std::make_shared<StrictMock<mock_job_evaluator>>();

	const char *id = "eval_123";
	const char *job_url = "http://dot.com/a.tar.gz";
	const char *result_url = "http://dot.com/results/123";

	eval_response response(id, "OK");

	EXPECT_CALL(*evaluator,
		evaluate(AllOf(Field(&eval_request::job_id, StrEq(id)),
			Field(&eval_request::job_url, StrEq(job_url)),
			Field(&eval_request::result_url, StrEq(result_url)))))
		.Times(1)
		.WillOnce(Return(response));

	job_receiver receiver(context, evaluator, nullptr);
	std::thread r([&receiver]() { receiver.start_receiving(); });

	socket.send("eval", 4, ZMQ_SNDMORE);
	socket.send(id, 8, ZMQ_SNDMORE);
	socket.send(job_url, 23, ZMQ_SNDMORE);
	socket.send(result_url, 26, 0);

	std::this_thread::sleep_for(std::chrono::milliseconds(1));

	zmq::message_t msg;
	bool retval;

	retval = socket.recv(&msg, ZMQ_NOBLOCK);
	ASSERT_TRUE(retval);
	ASSERT_EQ("done", std::string((char *) msg.data(), msg.size()));
	ASSERT_TRUE(msg.more());

	retval = socket.recv(&msg, ZMQ_NOBLOCK);
	ASSERT_TRUE(retval);
	ASSERT_EQ(id, std::string((char *) msg.data(), msg.size()));
	ASSERT_TRUE(msg.more());

	retval = socket.recv(&msg, ZMQ_NOBLOCK);
	ASSERT_TRUE(retval);
	ASSERT_EQ("OK", std::string((char *) msg.data(), msg.size()));
	ASSERT_TRUE(msg.more());

	retval = socket.recv(&msg, ZMQ_NOBLOCK);
	ASSERT_TRUE(retval);
	ASSERT_EQ("", std::string((char *) msg.data(), msg.size()));
	ASSERT_FALSE(msg.more());

	context->close();
	r.join();
}

TEST(job_receiver, incomplete_msg)
{
	auto context = std::make_shared<zmq::context_t>(1);
	zmq::socket_t socket(*context, ZMQ_PAIR);
	socket.bind("inproc://" + JOB_SOCKET_ID);

	// We don't expect any calls
	auto evaluator = std::make_shared<StrictMock<mock_job_evaluator>>();

	job_receiver receiver(context, evaluator, nullptr);
	std::thread r([&receiver]() { receiver.start_receiving(); });

	socket.send("eval", 4, ZMQ_SNDMORE);
	socket.send("foo", 3, ZMQ_SNDMORE);
	socket.send("foo", 3, 0);

	std::this_thread::sleep_for(std::chrono::milliseconds(1));
	context->close();
	r.join();
}

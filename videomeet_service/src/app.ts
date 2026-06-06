/**
 * @createdBy Selvakani
 * @createdOn 27th April 2021
 */

import { createServer } from "http";
import { Server, Socket } from "socket.io";
import { RedisClient } from "redis";
import { createAdapter, RedisAdapter } from "socket.io-redis";
import { config } from "./config";

interface User {
  user: String;
  conId: String;
}

interface Map {
  [key: string]: [value: User];
}

const httpServer = createServer();
const pubClient = new RedisClient({
  // host: '10.30.3.28',
  // port: 6379
  host: config.redis_client.host,
  port: config.redis_client.port,
});
const subClient = pubClient.duplicate();

const part: Map = {};
const waiting: Map = {};
const isHostJoined = {};

const io = new Server(httpServer, {
  cors: {
    origin: "*",
  },
});

io.adapter(createAdapter({ pubClient, subClient }));

io.on("connection", (socket: Socket) => {
  console.log("connected");
  //console.log(socket.handshake.query);
  const meetingId = socket.handshake.query.roomId || "";
  const userName = socket.handshake.query.userName || "";
  part[meetingId.toString()] = part[meetingId.toString()] || [];
  waiting[meetingId.toString()] = waiting[meetingId.toString()] || [];
  //socket.join("room1" + socket.handshake.query.userId);
  console.log(meetingId);
  if (userName === "host") {
    socket.join(meetingId);
    io.of("/")
      .to(meetingId)
      .emit("newChatMessage", {
        body: "Joined the meeting",
        senderId: socket.id,
        userName: userName,
      });
    io.of("/")
      .to(meetingId)
      .emit("command", {
        participantsmsg: part[meetingId.toString()],
        waitingRoommsg: waiting[meetingId.toString()],
      });
  } else {
    let li = waiting[meetingId.toString()] || [];
    let user: User = {
      user: userName.toString(),
      conId: socket.id,
    };
    li.push(user);
    waiting[meetingId.toString()] = li;
    part[meetingId.toString()] = part[meetingId.toString()] || [];
    io.of("/")
      .to(meetingId)
      .emit("command", {
        participantsmsg: part[meetingId.toString()],
        waitingRoommsg: waiting[meetingId.toString()],
      });
  }

  console.log(io.of("/").adapter.rooms);

  socket.on("newChatMessage", (m: any) => {
    const adapter: RedisAdapter = io.of("/").adapter as RedisAdapter;
    //  console.log('[server](message): %s', JSON.stringify(m));
    //pubClient.lpush(meetingId.toString(),m);
    io.of("/").to(meetingId).emit("newChatMessage", m);
  });

  socket.on("addparticipants", (m: any) => {
    const adapter: RedisAdapter = io.of("/").adapter as RedisAdapter;
    //console.log('[server](message): %s', JSON.stringify(m));
    //pubClient.lpush(meetingId.toString(),m);
    io.of("/").to(meetingId).emit("newChatMessage", m);
  });

  socket.on("command", async (m: any) => {
    const adapter: RedisAdapter = io.of("/").adapter as RedisAdapter;
    console.log("Inside command");
    if (m.type === "movetowaitroom") {
      console.log(m);
      adapter
        .remoteLeave(m.socketId, meetingId.toString())
        .then(() => {
          io.of("/")
            .to(meetingId)
            .emit("newChatMessage", {
              body: "moved to waiting room",
              senderId: socket.id,
              userName: m.userName,
            });
          let user = part[meetingId.toString()].filter(
            (x) => x.conId === m.socketId
          )[0];
          waiting[meetingId.toString()].push(user);
          part[meetingId.toString()].splice(
            part[meetingId.toString()].indexOf(user),
            1
          );
          io.of("/")
            .to(m.socketId)
            .emit("command", { isInWaitingRoommsg: true, to: m.socketId });
          io.of("/")
            .to(meetingId)
            .emit("command", {
              participantsmsg: part[meetingId.toString()],
              waitingRoommsg: waiting[meetingId.toString()],
            });
        })
        .catch((x) => console.log(x));
    }


    console.log(io.of("/").adapter.rooms);

    socket.on('newChatMessage', (m: any) => {
      const adapter: RedisAdapter = io.of('/').adapter as RedisAdapter;
      //  console.log('[server](message): %s', JSON.stringify(m));
      //pubClient.lpush(meetingId.toString(),m);
      io.of("/").to(meetingId).emit("newChatMessage", m);

    });

    socket.on('addparticipants', (m: any) => {
      const adapter: RedisAdapter = io.of('/').adapter as RedisAdapter;
      //console.log('[server](message): %s', JSON.stringify(m));
      //pubClient.lpush(meetingId.toString(),m);
      io.of("/").to(meetingId).emit("newChatMessage", m);

    });


    socket.on('command', async (m: any) => {
      const adapter: RedisAdapter = io.of('/').adapter as RedisAdapter;
      console.log("Inside command");
      if (m.type === "movetowaitroom") {

        console.log(m);
        adapter.remoteLeave(m.socketId, meetingId.toString())
          .then(
            () => {
              io.of("/").to(meetingId).emit("newChatMessage", { body: 'moved to waiting room', senderId: socket.id, userName: m.userName })
              let user = part[meetingId.toString()].filter(x => x.conId === m.socketId)[0];
              waiting[meetingId.toString()].push(user);
              part[meetingId.toString()].splice(part[meetingId.toString()].indexOf(user), 1);
              io.of("/").to(m.socketId).emit('command', { 'isInWaitingRoommsg': true, 'to': m.socketId });
              io.of("/").to(meetingId).emit("command", { 'participantsmsg': part[meetingId.toString()], 'waitingRoommsg': waiting[meetingId.toString()] });
            })
          .catch(x => console.log(x));
      }
      if (m.type === "joinmeeting") {
        console.log("Inside join meeting");
        console.log("Join meetinf >>>>>>>>>>>>>>>>>>>>>>", m);
        adapter.remoteJoin(m.socketId, meetingId.toString())
          .then(
            () => {
              io.of("/").to(meetingId).emit("newChatMessage", { body: 'Joined the meeting', senderId: socket.id, userName: m.userName });
              let user = waiting[meetingId.toString()].filter(x => x.conId === m.socketId)[0];
              part[meetingId.toString()].push(user);
              waiting[meetingId.toString()].splice(waiting[meetingId.toString()].indexOf(user), 1);
              io.of("/").to(meetingId).emit("command", { 'participantsmsg': part[meetingId.toString()], 'waitingRoommsg': waiting[meetingId.toString()] });
              io.of("/").to(m.socketId).emit('command', { 'isInWaitingRoom': false, 'to': m.socketId })
            })
          .catch(x => console.log(x));
      }
      if (m.type === "removeFromMeeting") {
        console.log("Inside disconect meeting");
        adapter.remoteDisconnect(m.socketId, m.roomId)
          .then(() => {
            io.of("/").to(meetingId).emit("newChatMessage", { body: 'removed from the meeting', senderId: socket.id, userName: m.userName });

          }
          )
          .catch((x: any) => console.log(x));
      }
      if (m.type === "removeMultipleParticipants") {
        adapter.remoteDisconnect(m.socketId, m.roomId)
          .then(() => {
            io.of("/").to(meetingId).emit("newChatMessage", { body: 'removed from the meeting', senderId: socket.id, userName: m.userName });

          }
          )
          .catch((x: any) => console.log(x));
      }

    });
    });
  });

  const port = process.env.socketPort || 3020;

  httpServer.listen(port);
  console.log("listen in port " + port);

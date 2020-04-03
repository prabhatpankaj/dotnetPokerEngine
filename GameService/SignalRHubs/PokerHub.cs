﻿using GameService.Models;
using Microsoft.AspNetCore.SignalR;
using System;
using System.Threading.Tasks;

namespace GameService.Hubs
{
    public class PokerHub : Hub
    {
        private IGame _game;

        public PokerHub(IGame gameState)
        {
            _game = gameState;
        }

        public async Task SendMessage(string user, string message)
        {
            await Clients.All.SendAsync("ReceiveMessage", Context.ConnectionId, message);
            await Clients.All.SendAsync("GameState", _game);
        }

        public async Task ExecutePlayerAction(PlayerEvent playerAction)
        {
            // Do the action
            await _game.ProcessClientAction(playerAction);
            
        }

        public async Task PlayerWentInactive(int playerId)
        {
            throw new NotImplementedException();
        }

        public override async Task OnConnectedAsync()
        {
            var player = new GamePlayer()
            {
                Id = Context.ConnectionId
            };

            await _game.AddPlayer(player);

            await base.OnConnectedAsync();
        }

        /**
         * If the client disconnects intentionally (by calling connection.stop(), for example),
         * the exception parameter will be null. However, if the client is disconnected due to an
         * error (such as a network failure), the exception parameter will contain an exception describing the failure.
         */

        public override async Task OnDisconnectedAsync(Exception exception)
        {
            //await Groups.RemoveFromGroupAsync(Context.ConnectionId, "SignalR Users");
            if (exception == null)
            {
                await _game.RemovePlayer(Context.ConnectionId);
            }
            await base.OnDisconnectedAsync(exception);
        }
    }
}
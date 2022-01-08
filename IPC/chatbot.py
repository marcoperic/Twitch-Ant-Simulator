'''
Copyright 2017 Amazon.com, Inc. or its affiliates. All Rights Reserved.

Licensed under the Apache License, Version 2.0 (the "License"). You may not use this file except in compliance with the License. A copy of the License is located at

    http://aws.amazon.com/apache2.0/

or in the "license" file accompanying this file. This file is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied. See the License for the specific language governing permissions and limitations under the License.
'''

import sys
import irc.bot
import requests
import threading
import time
import _thread

from server import Server

class TwitchBot(irc.bot.SingleServerIRCBot):
    def __init__(self, username, client_id, token, channel):
        self.client_id = client_id
        self.token = token
        self.channel = '#' + channel
        self.cvar = Server()

        # Get the channel id, we will need this for v5 API calls
        url = 'https://api.twitch.tv/kraken/users?login=' + channel
        headers = {'Client-ID': client_id, 'Accept': 'application/vnd.twitchtv.v5+json'}
        r = requests.get(url, headers=headers).json()
        self.channel_id = r['users'][0]['_id']

        # Create IRC bot connection
        server = 'irc.chat.twitch.tv'
        port = 6667
        print('Connecting to ' + server + ' on port ' + str(port) + '...')
        irc.bot.SingleServerIRCBot.__init__(self, [(server, port, 'oauth:'+token)], username, username)
        

    def on_welcome(self, c, e):
        print ('Joining ' + self.channel)

        # You must request specific capabilities before you can use them
        c.cap('REQ', ':twitch.tv/membership')
        c.cap('REQ', ':twitch.tv/tags')
        c.cap('REQ', ':twitch.tv/commands')
        c.join(self.channel)

    def on_pubmsg(self, c, e):

        # If a chat message starts with an exclamation point, try to run it as a command
        if e.arguments[0][:1] == '!':
            cmd = e.arguments[0].split(' ')[0][1:]
            print ('Received command: ' + cmd)
            self.do_command(e, cmd, e.arguments[0].split(' '))
        return

    def do_command(self, e, cmd, args):
        c = self.connection
        # Poll the API to get current game.
        if cmd == "game":
            url = 'https://api.twitch.tv/kraken/channels/' + self.channel_id
            headers = {'Client-ID': self.client_id, 'Accept': 'application/vnd.twitchtv.v5+json'}
            r = requests.get(url, headers=headers).json()
            c.privmsg(self.channel, r['display_name'] + ' is currently playing ' + r['game'])

        # Poll the API the get the current status of the stream
        elif cmd == "title":
            url = 'https://api.twitch.tv/kraken/channels/' + self.channel_id
            headers = {'Client-ID': self.client_id, 'Accept': 'application/vnd.twitchtv.v5+json'}
            r = requests.get(url, headers=headers).json()
            c.privmsg(self.channel, r['display_name'] + ' channel title is currently ' + r['status'])

        # Provide basic information to viewers for specific commands
        # elif cmd == "raffle":
        #     message = "This is an example bot, replace this text with your raffle text."
        #     c.privmsg(self.channel, message)
        # elif cmd == "schedule":
        #     message = "This is an example bot, replace this text with your schedule text."            
        #     c.privmsg(self.channel, message)

        # The command was not recognized
        elif cmd == "spawn":
            # c.privmsg(self.channel, "Did not understand command: " + cmd)
            print("Send to command queue")
            print(args)
            cmd = "SP"
            self.cvar.command_queue.append(cmd + args[1])

def main():
    if len(sys.argv) == 5:
        print("Usage: twitchbot <username> <client id> <token> <channel>")
        sys.exit(1)

    username  = 'mperic_chatbot'
    client_id = 'gp762nuuoqcoxypju8c569th9wz7q5'
    token     = '57mmemwwnb0p1b6z366fv1cuw1tb03'
    channel   = 'mperic'

    
    bot = TwitchBot(username, client_id, token, channel)
    bot.cvar.init_threading()
    bot.start()

if __name__ == "__main__":
    main()
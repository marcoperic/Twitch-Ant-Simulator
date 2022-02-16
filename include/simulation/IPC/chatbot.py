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

        # Get the channel id for v6 Helix auth
        url = 'https://api.twitch.tv/helix/users?login=' + channel
        headers = {'Client-ID': client_id, 'Authorization': 'Bearer ' + token}
        r = requests.get(url, headers=headers).json()
        self.channel_id = r['data'][0]['id']
        # print(self.channel_id)

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

    # def create_poll(self):
    #     url = "https://api.twitch.tv/helix/polls"
    #     headers = {'Client-ID': self.client_id, 'title': "test", 'choices':['op1', 'op2'], 'choice.title': 'what?', 'duration': 15}
    #     r = requests.post(url, headers)

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

        elif cmd == "spawn":
            # c.privmsg(self.channel, "Did not understand command: " + cmd)
            cmd = "S"
            if (len(args) > 2 or len(args) == 1):
                c.privmsg(self.channel, "Please try !spawn color")
            else:
                if (args[1] in self.cvar.colors):
                    cmd += self.cvar.colors[args[1]]
                    self.cvar.pushCmd(cmd)
                else:
                    c.privmsg(self.channel, "Invalid color. Try another one, or check the colorblind panel below!")

        elif cmd == "feed":
            # Spawn food near a certain colony.
            cmd = "F"
            if (len(args) > 2):
                c.privmsg(self.channel, "Too many arguments! Please try !feed color")
            else:
                if (args[1] in self.cvar.colors):
                    cmd += self.cvar.colors[args[1]]
                    self.cvar.pushCmd(cmd)
                else:
                    c.privmsg(self.channel, "Too many arguments! Please try !feed color")
                    

        elif cmd == "event":
            # Cause good or bad event near colony.
            cmd = "E"

            if (len(args) > 2):
                c.privmsg(self.channel, "Too many arguments! Please try !event eventname")
            else:
                if (args[1] in self.cvar.events):
                    print()

def main():
    if len(sys.argv) == 5:
        print("Usage: twitchbot <username> <client id> <token> <channel>")
        sys.exit(1)

    username  = 'mperic_chatbot'
    client_id = 'gp762nuuoqcoxypju8c569th9wz7q5'
    token     = '3af8zd1ck6m6ddiq5qnf9zcugbjb34'
    channel   = 'mperic'

    
    bot = TwitchBot(username, client_id, token, channel)
    bot.cvar.init_threading()
    bot.start()
    
    bot.create_poll()

if __name__ == "__main__":
    main()
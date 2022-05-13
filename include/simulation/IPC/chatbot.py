import sys
import irc.bot
import requests
import threading
import time
import _thread
import random

from server import Server

POLL_DURATION = 20 # Temporary - default 60?

class TwitchBot(irc.bot.SingleServerIRCBot):
    def __init__(self, username, client_id, token, channel):
        self.client_id = client_id
        self.token = token
        self.channel = '#' + channel
        self.cvar = Server(self)

        # Get the channel id for v6 Helix auth
        url = 'https://api.twitch.tv/helix/users?login=' + channel
        headers = {'Client-ID': client_id, 'Authorization': 'Bearer ' + token}
        r = requests.get(url, headers=headers).json()
        self.channel_id = r['data'][0]['id']

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

    def create_poll(self, polldata):
        url = 'https://api.twitch.tv/helix/polls'
        headers = {'Client-ID': self.client_id, 'Authorization': 'Bearer ' + self.token, 'Content-Type': 'application/json'}
        data = {}
        
        if (polldata):
            data = { "broadcaster_id":self.channel_id, "title": polldata['title'], "choices": polldata['choices'], "channel_points_voting_enabled":True, "channel_points_per_vote":100, "duration":POLL_DURATION }
        else:
            data = { "broadcaster_id":self.channel_id, "title":"Sample poll", "choices":[{ "title":"First" }, { "title":"Second" }], "channel_points_voting_enabled":True, "channel_points_per_vote":100, "duration":POLL_DURATION }
        
        r = requests.post(url=url, headers=headers, json=data).json()
        print(r)

        # Create thread to wait for poll to finish and then send command to client.
        _thread.start_new_thread(self.handlePollResult, ())


    def handlePollResult(self):
        print('Poll handler opened.')
        time.sleep(POLL_DURATION + 5) # Add 5 seconds to account for any API delays, etc...
        url = 'https://api.twitch.tv/helix/polls?broadcaster_id=' + self.channel_id
        headers = {'Client-ID': self.client_id, 'Authorization': 'Bearer ' + self.token}
        r = requests.get(url=url, headers=headers).json()['data'][0]
        print(r)

        # Find which colony had most votes
        winner = self.getPollWinner(r)
        # Use poll_codes to know what the poll was for. Send command to client. 
        self.cvar.pushCmd(self.getCommandType(winner))

        self.cvar.poll_info.clear()
        return # Return silent terminates thread.

    def getCommandType(self, winner):
        token = self.cvar.poll_info[0].split("_")
        return token[0] + winner + str(token[1])

    def getPollWinner(self, r):
        options = r['choices'] # Shuffle so that red isn't chosen by default when tied.
        random.shuffle(options)
        winner = options[0]
        max = -1
        for c in options:
            if (c['votes'] > max):
                max = c['votes']
                winner = c['title']

        return winner[0] # return the first character - all we care about

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
                    cmd += '1'
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
                    cmd += '1'
                    self.cvar.pushCmd(cmd)
                else:
                    c.privmsg(self.channel, "Too many arguments! Please try !feed color")

        # Usage: !debug spawn 100 -- spawns 100 ants randomly
        elif cmd == 'debug':
            cmd = args[1]
            quantity = args[2]

            if cmd == 'spawn':
                cmd = 'S'
            elif cmd == 'feed':
                cmd = 'F'

            
            for i in range(0, int(quantity)):
                temp = cmd
                temp += self.cvar.colors[random.choice(list(self.cvar.colors))]
                temp += '1'
                print('DEBUG: ' + temp)
                self.cvar.pushCmd(temp)


def main():
    if len(sys.argv) == 5:
        print("Usage: twitchbot <username> <client id> <token> <channel>")
        sys.exit(1)


    #refresh token 18yok3pd703svpr6xqule44kq4axh3g4z4clamjoyhw26zzhol
    username  = 'mperic_chatbot'
    client_id = 'gp762nuuoqcoxypju8c569th9wz7q5'
    token     = '5bhttb8p6p1gojii2st9jdb423vmes'
    channel   = 'mperic'

    
    bot = TwitchBot(username, client_id, token, channel)
    bot.cvar.init_threading()
    bot.start()

if __name__ == "__main__":
    main()
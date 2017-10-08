int lengthOfLongestSubstring(string s) {
        int len = 1;
        int max = 1;
        int j = 0;
        
        string sub = s.substr(0, 1);
        
        for (int i = 1; i < s.size(); ++i) {
            string cur = s.substr(i, 1);
            if (sub.find(cur) != string::npos) {
                len = 1;
                j = i;
                sub = s.substr(j, 1);
            } else {
                len++;
                sub = s.substr(j, len);
                max = len > max ? len : max;
            }
        }
        
        return max;
    }

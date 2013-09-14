#include "../delimiters.h"

TEST(Delims00, { Delimiters d; return d.AtDelimiter(S" ", true)  == true;  })
TEST(Delims01, { Delimiters d; return d.AtDelimiter(S" ", false) == false; })
TEST(Delims02, { Delimiters d; return d.AtDelimiter(S"",  true)  == true;  })
TEST(Delims03, { Delimiters d; return d.AtDelimiter(S"",  false) == true;  })
TEST(Delims04, { Delimiters d; return d.AtDelimiter(S",", false) == false; })
TEST(Delims05, { Delimiters d; return d.AtDelimiter(S",", true)  == false; })

TEST(Delims06a, { Delimiters d; d.Set(','); return d.AtDelimiter(S",",  true) == true; })
TEST(Delims07a, { Delimiters d; d.Set(','); return d.AtDelimiter(S",", false) == true; })
TEST(Delims08a, { Delimiters d; d.Set(','); return d.AtDelimiter(S" ,",  true) == true; })
TEST(Delims09a, { Delimiters d; d.Set(','); return d.AtDelimiter(S" ,", false) == false; })

TEST(Delims06j, { Delimiters d; d.Set(','); return d.AtDelimiter(S"",  true) == true; })
TEST(Delims07j, { Delimiters d; d.Add(','); return d.AtDelimiter(S"", false) == true; })
TEST(Delims08j, { Delimiters d; d.SetOptional(','); return d.AtDelimiter(S"",  true) == true; })
TEST(Delims09j, { Delimiters d; d.AddOptional(','); return d.AtDelimiter(S"", false) == true; })

TEST(Delims06b, { Delimiters d; d.Set(","); return d.AtDelimiter(S",",  true) == true; })
TEST(Delims07b, { Delimiters d; d.Set(","); return d.AtDelimiter(S",", false) == true; })
TEST(Delims08b, { Delimiters d; d.Set(","); return d.AtDelimiter(S" ,",  true) == true; })
TEST(Delims09b, { Delimiters d; d.Set(","); return d.AtDelimiter(S" ,", false) == false; })

TEST(Delims06i, { Delimiters d; d.Set(","); return d.AtDelimiter(S"#",  true) == false; })
TEST(Delims07i, { Delimiters d; d.Set("#"); return d.AtDelimiter(S",", false) == false; })
TEST(Delims08i, { Delimiters d; d.Set("'"); return d.AtDelimiter(S"*",  true) == false; })
TEST(Delims09i, { Delimiters d; d.Set("*"); return d.AtDelimiter(S"'", false) == false; })

TEST(Delims06c, { Delimiters d; d.Set(",,"); return d.AtDelimiter(S",,",  true) == true; })
TEST(Delims07c, { Delimiters d; d.Set(",,"); return d.AtDelimiter(S",,", false) == true; })
TEST(Delims08c, { Delimiters d; d.Set(",,"); return d.AtDelimiter(S" ,,",  true) == true; })
TEST(Delims09c, { Delimiters d; d.Set(",,"); return d.AtDelimiter(S" ,,", false) == false; })
TEST(Delims10c, { Delimiters d; d.Set(",,"); return d.AtDelimiter(S",",  true) == false; })
TEST(Delims11c, { Delimiters d; d.Set(",,"); return d.AtDelimiter(S",", false) == false; })
TEST(Delims12c, { Delimiters d; d.Set(",,"); return d.AtDelimiter(S",'",  true) == false; })
TEST(Delims13c, { Delimiters d; d.Set(",,"); return d.AtDelimiter(S",'", false) == false; })

TEST(Delims06d, { Delimiters d; d.Set(','); d.Set(';'); return d.AtDelimiter(S",",  true) == false; })
TEST(Delims07d, { Delimiters d; d.Set(','); d.Set(';'); return d.AtDelimiter(S",", false) == false; })
TEST(Delims08d, { Delimiters d; d.Set(','); d.Set(';'); return d.AtDelimiter(S";",  true) == true; })
TEST(Delims09d, { Delimiters d; d.Set(','); d.Set(';'); return d.AtDelimiter(S";", false) == true; })

TEST(Delims06e, { Delimiters d; d.Set(","); d.Add(";"); return d.AtDelimiter(S",",  true) == true; })
TEST(Delims07e, { Delimiters d; d.Set(","); d.Add(";"); return d.AtDelimiter(S",", false) == true; })
TEST(Delims08e, { Delimiters d; d.Set(","); d.Add(";"); return d.AtDelimiter(S";",  true) == true; })
TEST(Delims09e, { Delimiters d; d.Set(","); d.Add(";"); return d.AtDelimiter(S";", false) == true; })

TEST(Delims06f, { Delimiters d; d.Add(','); d.Set(';'); return d.AtDelimiter(S",",  true) == false; })
TEST(Delims07f, { Delimiters d; d.Add(","); d.Set(";"); return d.AtDelimiter(S",", false) == false; })
TEST(Delims08f, { Delimiters d; d.Add(","); d.Set(";"); return d.AtDelimiter(S";",  true) == true; })
TEST(Delims09f, { Delimiters d; d.Add(','); d.Set(';'); return d.AtDelimiter(S";", false) == true; })

TEST(Delims06g, { Delimiters d; d.Add(","); d.Add(";"); return d.AtDelimiter(S",",  true) == true; })
TEST(Delims07g, { Delimiters d; d.Add(','); d.Add(';'); return d.AtDelimiter(S",", false) == true; })
TEST(Delims08g, { Delimiters d; d.Add(','); d.Add(';'); return d.AtDelimiter(S";",  true) == true; })
TEST(Delims09g, { Delimiters d; d.Add(","); d.Add(";"); return d.AtDelimiter(S";", false) == true; })


TEST(Delims06h, { Delimiters d; d.Set(','); d.Set(';'); return d.AtDelimiter(S",", false) == false; })
TEST(Delims07h, { Delimiters d; d.Set(','); d.Set(';'); return d.AtDelimiter(S";", false) == true; })
TEST(Delims08h, { Delimiters d; d.Set(','); d.AddOptional(';'); return d.AtDelimiter(S",", false) == true; })
TEST(Delims09h, { Delimiters d; d.Set(','); d.AddOptional(';'); return d.AtDelimiter(S";", false) == true; })
TEST(Delims10h, { Delimiters d; d.SetOptional(','); d.Add(';'); return d.AtDelimiter(S",", false) == true; })
TEST(Delims11h, { Delimiters d; d.SetOptional(','); d.Add(';'); return d.AtDelimiter(S";", false) == true; })
TEST(Delims12h, { Delimiters d; d.SetOptional(','); d.SetOptional(';'); return d.AtDelimiter(S",", false) == false; })
TEST(Delims13h, { Delimiters d; d.SetOptional(','); d.SetOptional(';'); return d.AtDelimiter(S";", false) == true; })





TEST(Delims14, { Delimiters d; return d.SkipDelimiters(S" Q") && *CUR == 'Q'; })
TEST(Delims15, { Delimiters d; return d.SkipDelimiters(S"") && *CUR == '\0'; })
TEST(Delims16, { Delimiters d; return d.SkipDelimiters(S" \t5") && *CUR == '5'; })
TEST(Delims17, { Delimiters d; return d.SkipDelimiters(S"\n,") && *CUR == ','; })
TEST(Delims18, { Delimiters d; d.Add(','); return d.SkipDelimiters(S"\r,") && *CUR == '\0'; })
TEST(Delims19, { Delimiters d; d.Add(','); return d.SkipDelimiters(S" , ") && *CUR == '\0'; })
TEST(Delims20, { Delimiters d; d.Add(';'); return d.SkipDelimiters(S";hi") && *CUR == 'h'; })
TEST(Delims21, { Delimiters d; d.Add("hy"); return d.SkipDelimiters(S"hya") && *CUR == 'a'; })
TEST(Delims29, { Delimiters d; d.Add("hy"); return d.SkipDelimiters(S"hyhya") && *CUR == 'h'; })
TEST(Delims22, { Delimiters d; d.AddOptional("hy"); return d.SkipDelimiters(S"hyha") && *CUR == 'h'; })
TEST(Delims23, { Delimiters d; d.AddOptional("hy"); return d.SkipDelimiters(S"hyhyhyb") && *CUR == 'b'; })
TEST(Delims24, { Delimiters d; d.AddOptional("hy"); return d.SkipDelimiters(S" hy hy hy b") && *CUR == 'b'; })
TEST(Delims25, { Delimiters d; d.AddOptional("hy"); d.Add("F"); return d.SkipDelimiters(S" hy F hy hy z") && *CUR == 'F'; })
TEST(Delims30, { Delimiters d; d.AddOptional("hy"); d.Add("F"); return d.SkipDelimiters(S" F hyt hy hy z") && *CUR == 'h' && *(CUR + 2) == 't'; })
TEST(Delims26, { Delimiters d; d.AddOptional("hy"); d.Set("F"); return !d.SkipDelimiters(S" hy F hy F hy y"); })
TEST(Delims27, { Delimiters d; d.AddOptional("hy"); d.AddOptional("F"); return d.SkipDelimiters(S" hy F hy F hy x") && *CUR == 'F'; })
TEST(Delims28, { Delimiters d; d.Add("hy"); d.AddOptional("F"); return d.SkipDelimiters(S"F hyt F hy w") && *CUR == 'h' && *(CUR + 2) == 't'; })


TEST(Delims31, { Delimiters d; d.Add(","); d.Add(";"); return d.AtDelimiter(S";", true) == true; })
TEST(Delims32, { Delimiters d; d.Add(","); d.Add(";"); d.Remove(","); return d.AtDelimiter(S";", true) == true; })
TEST(Delims33, { Delimiters d; d.Add(","); d.Add(";"); d.Remove(";"); return d.AtDelimiter(S";", true) == false; })
TEST(Delims34, { Delimiters d; d.Add(","); d.Add(";"); d.Remove(","); return d.AtDelimiter(S",", true) == false; })
TEST(Delims35, { Delimiters d; d.Add(","); d.Add(";"); d.Remove(";"); return d.AtDelimiter(S",", true) == true; })



# sjis-conv.p6 --- convert Unicode's "obsolete" Shift-JIS data to a C++-y format

use v6;

my @flines = slurp("SHIFTJIS.TXT").lines;

my %lilmap := :{};
my %bigmap := :{};

for @flines {
    next if $_ ~~ /^ <.ws> '#'/;

    my $sjis = +$_.words[0];
    my $utf8 = +$_.words[1];

    next if $sjis == $utf8; # skip no-op maps

    if $sjis <= 0xFF { # single-byte sequence
        %lilmap{$sjis} = $utf8;
    } else { # two-byte
        my $sb1 = $sjis +> 8;
        my $sb2 = $sjis +& 0xFF;

        unless %bigmap{$sb1}:exists {
            %bigmap{$sb1} := :{};
        }

        %bigmap{$sb1}{$sb2} = $utf8;
    }
}

my $ofile = open("sjtable.inc", :w);

$ofile.print("std::map<uint8_t, uint32_t> SJIS_singleTable\{\n");

for %lilmap.sort {
    if $++ %% 4 {
        $ofile.print("\n    ");
    } else {
        $ofile.print(" ");
    }

    $ofile.print("\{ ", $_.key.fmt("0x%02X"), ", ", $_.value.fmt("0x%08X"), " \},");
}

$ofile.print("\};\n\n");

$ofile.print("std::map<uint8_t, std::map<uint8_t, uint32_t>> SJIS_doubleTable\{\n");

for %bigmap.sort {
    $ofile.print("    \{ ", $_.key.fmt("0x%02X"), ", \{");

    for $_.value.sort {
        if $++ %% 4 {
            $ofile.print("\n        ");
        } else {
            $ofile.print(" ");
        }

        $ofile.print("\{ ", $_.key.fmt("0x%02X"), ", ", $_.value.fmt("0x%08X"), " \},");
    }

    $ofile.print("    \},\n");
}

$ofile.print("\};");

$ofile.close();
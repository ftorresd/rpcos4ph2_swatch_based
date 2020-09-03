package internal

import "net"

// XDAQ does not listen on 0.0.0.0, so a proxy to localhost does not work
// we need the same IP that xdaq detected
func getLocalIP() (net.IP, error) {
	conn, err := net.Dial("udp", "8.8.8.8:80")
	if err != nil {
		return nil, err
	}
	defer conn.Close()

	localAddr := conn.LocalAddr().(*net.UDPAddr)

	return localAddr.IP, nil

	// ifaces, err := net.Interfaces()
	// if err != nil {
	// 	return nil, err
	// }
	// for _, i := range ifaces {
	// 	addrs, err := i.Addrs()
	// 	if err != nil {
	// 		return nil, err
	// 	}
	// 	for _, addr := range addrs {
	// 		switch v := addr.(type) {
	// 		case *net.IPNet:
	// 			if !v.IP.IsLoopback() {
	// 				return v.IP, nil
	// 			}
	// 		case *net.IPAddr:
	// 			if !v.IP.IsLoopback() {
	// 				return v.IP, nil
	// 			}
	// 		}
	// 	}
	// }
	// return nil, errors.New("could not find local IP")
}

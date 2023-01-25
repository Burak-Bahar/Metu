import java.util.ArrayList;

public class PlaylistTree {
	
	public PlaylistNode primaryRoot;		//root of the primary B+ tree
	public PlaylistNode secondaryRoot;	//root of the secondary B+ tree
	public PlaylistTree(Integer order) {
		PlaylistNode.order = order;
		primaryRoot = new PlaylistNodePrimaryLeaf(null);
		primaryRoot.level = 0;
		secondaryRoot = new PlaylistNodeSecondaryLeaf(null);
		secondaryRoot.level = 0;
	}
	
	public void addSong(CengSong song) {
		add_to_primary(song);
		add_to_secondary(song);

	}

	public void add_to_secondary(CengSong song) {
		PlaylistNode tmp = secondaryRoot;
		while (tmp.getType() != PlaylistNodeType.Leaf){
			int i = ((PlaylistNodeSecondaryIndex)tmp).get_place(song.genre());
			tmp = ((PlaylistNodeSecondaryIndex)tmp).getChildrenAt(i);
		}
		PlaylistNodeSecondaryLeaf leaf = ((PlaylistNodeSecondaryLeaf) tmp);
		int position = 0;
		for(int i = 0 ; i < leaf.genreCount() ; i++){
			if((song.genre()).compareToIgnoreCase(leaf.genreAtIndex(i)) > 0 ){
				position += 1;
			}
		}

		if(leaf.genreCount() < 2* PlaylistNode.order || exists(song, leaf)){
			leaf.addSong(position, song);
		}
		else {
			PlaylistNodeSecondaryIndex parent;
			int middle = leaf.genreCount()/2;
			//exactly middle so we need to copy up. will add song to right side
			if((song.genre()).compareToIgnoreCase(leaf.genreAtIndex(middle)) < 0  && (song.genre()).compareToIgnoreCase(leaf.genreAtIndex(middle-1)) > 0 ){
				parent = s_c_up(leaf, 0, song);
			}
			// left side. need to up middle
			else if ((song.genre()).compareToIgnoreCase(leaf.genreAtIndex(middle)) < 0){
				parent = s_c_up(leaf , 1, song);
			}
			// right side. need to up middle
			else {
				parent = s_c_up(leaf, 2, song);
			}
			while (parent.genreCount() > 2* PlaylistNode.order){
				// push up
				parent = s_p_up(parent);
			}
			if(parent.getParent()== null){
				secondaryRoot = parent;
			}
		}
	}

	public boolean exists(CengSong song, PlaylistNodeSecondaryLeaf leaf) {
		for (int i=0; i < leaf.genreCount(); i++) {
			if (leaf.genreAtIndex(i).equalsIgnoreCase(song.genre())){
				return true;
			}
		}
		return false;
	}
	public PlaylistNodeSecondaryIndex s_c_up(PlaylistNodeSecondaryLeaf leaf, int mode, CengSong song){
		int mid_index = leaf.genreCount()/2;
		String mid_value = leaf.genreAtIndex(mid_index);
		PlaylistNodeSecondaryIndex parent = (PlaylistNodeSecondaryIndex) leaf.getParent();

		PlaylistNodeSecondaryLeaf left_node = new PlaylistNodeSecondaryLeaf(parent);
		PlaylistNodeSecondaryLeaf right_node = new PlaylistNodeSecondaryLeaf(parent);
		if(mode == 1){
			for(int i = 0; i < mid_index-1; i++) {
				ArrayList<CengSong> songs = leaf.songsAtIndex(i);
				for(int j = 0; j < songs.size(); j++) {
					left_node.addSong(i, songs.get(j));
				}
			}
			int size = leaf.genreCount();
			for(int i = mid_index-1; i < size; i++) {
				ArrayList<CengSong> songs = leaf.songsAtIndex(i);
				for(int j = 0; j < songs.size(); j++) {
					right_node.addSong(i- mid_index+1, songs.get(j));
				}
			}
		}
		else{
			for(int i = 0; i < mid_index; i++) {
				ArrayList<CengSong> songs = leaf.songsAtIndex(i);
				for(int j = 0; j < songs.size(); j++) {
					left_node.addSong(i, songs.get(j));
				}
			}

			int size = leaf.genreCount();
			for(int i = mid_index; i < size; i++) {
				ArrayList<CengSong> songs = leaf.songsAtIndex(i);
				for(int j = 0; j < songs.size(); j++) {
					right_node.addSong(i- mid_index, songs.get(j));
				}
			}
		}


		if(parent == null){ //for root
			parent = new PlaylistNodeSecondaryIndex(null);
			left_node.setParent(parent);
			right_node.setParent(parent);
			parent.set_child(0, left_node);
			parent.set_child(1, right_node);
			if(mode == 0){
				parent.set_value(0, song.genre());
			}
			else {
				parent.set_value(0, right_node.genreAtIndex(0));
			}

		}
		else { //id there are middle nodes
			int pos;
			// if newly added song goes up
			if (mode == 0){
				pos = parent.get_position(song.genre());
				parent.set_value(pos, song.genre());
			}
			// if newly added song doesn't go up
			else {
				pos = parent.get_position(mid_value);
				parent.set_value(pos, right_node.genreAtIndex(0));
			}

			parent.set_child(pos+1, right_node);
			parent.update_child(pos, left_node);

		}
		if (parent.getParent() == null){
			secondaryRoot = parent;
		}
		add_to_secondary(song);

		return parent;
	}

	public PlaylistNodeSecondaryIndex s_p_up(PlaylistNodeSecondaryIndex index_node){
		int mid_index = index_node.genreCount()/2;
		String mid_value = index_node.genreAtIndex(mid_index);
		PlaylistNodeSecondaryIndex parent = (PlaylistNodeSecondaryIndex) index_node.getParent();

		PlaylistNode node;

		PlaylistNodeSecondaryIndex left_node = new PlaylistNodeSecondaryIndex(parent);
		for (int i = 0; i < mid_index; i++) {
			node = index_node.getChildrenAt(i);
			node.setParent(left_node);
			left_node.set_child(i, node);
			left_node.set_value(i, index_node.genreAtIndex(i));
		}

		node = index_node.getChildrenAt(mid_index);
		node.setParent(left_node);
		left_node.set_child(mid_index, node);

		PlaylistNodeSecondaryIndex right_node = new PlaylistNodeSecondaryIndex(parent);
		for (int i = mid_index + 1 ; i < index_node.genreCount(); i++) {
			node = index_node.getChildrenAt(i);
			node.setParent(right_node);
			right_node.set_child(i - mid_index - 1, node);
			right_node.set_value(i - mid_index - 1, index_node.genreAtIndex(i));
		}

		node = index_node.getChildrenAt(index_node.genreCount());
		node.setParent(right_node);
		right_node.set_child(index_node.genreCount()- mid_index - 1 , node);

		if(parent == null){
			parent = new PlaylistNodeSecondaryIndex(null);
			left_node.setParent(parent);
			right_node.setParent(parent);
			parent.set_child(0, left_node);
			parent.set_child(1, right_node);
			parent.set_value(0, mid_value);
		}
		else{
			int pos = parent.get_position(mid_value);
			parent.set_value(pos, mid_value);
			parent.set_child(pos+1, right_node);
			parent.update_child(pos, left_node);
		}
		return parent;
	}
	public void add_to_primary(CengSong song){
		// primary
		PlaylistNode tmp = primaryRoot;
		while (tmp.getType() != PlaylistNodeType.Leaf){
			int i = ((PlaylistNodePrimaryIndex)tmp).get_place(song.audioId());
			tmp = ((PlaylistNodePrimaryIndex)tmp).getChildrenAt(i);
		}
		PlaylistNodePrimaryLeaf leaf = ((PlaylistNodePrimaryLeaf) tmp);
		int position = 0;
		for(int i = 0 ; i < leaf.songCount() ; i++){
			if(song.audioId() > leaf.audioIdAtIndex(i)){
				position += 1;
			}
		}

		if (leaf.songCount() < 2* PlaylistNode.order){
			leaf.addSong(position, song);
		}
		else {
			PlaylistNodePrimaryIndex parent;
			int middle = leaf.songCount()/2;
			//exactly middle so we need to copy up. will add song to right side
			if(song.audioId() < leaf.audioIdAtIndex(middle) && song.audioId() > leaf.audioIdAtIndex(middle-1)){
				parent = p_c_up(leaf, 0, song);
			}
			// left side. need to up middle
			else if (song.audioId() < leaf.audioIdAtIndex(middle)){
				parent = p_c_up(leaf , 1, song);
			}
			// right side. need to up middle
			else {
				parent = p_c_up(leaf, 2, song);
			}
			while (parent.audioIdCount() > 2* PlaylistNode.order){
				// push up
				parent = p_p_up(parent);
			}
			if(parent.getParent()== null){
				primaryRoot = parent;
			}
		}
	}

	public PlaylistNodePrimaryIndex p_c_up(PlaylistNodePrimaryLeaf leaf, int mode, CengSong song){
		int mid_index = leaf.songCount()/2;
		int mid_value = leaf.audioIdAtIndex(mid_index);
		PlaylistNodePrimaryIndex parent = (PlaylistNodePrimaryIndex) leaf.getParent();

		PlaylistNodePrimaryLeaf left_node = new PlaylistNodePrimaryLeaf(parent);
		PlaylistNodePrimaryLeaf right_node = new PlaylistNodePrimaryLeaf(parent);

		if (mode == 1){
			for(int i = 0; i < mid_index-1; i++) {
				left_node.addSong(i, leaf.songAtIndex(i));
			}
			int size = leaf.songCount();
			for(int i = mid_index-1; i < size; i++) {
				right_node.addSong(i - mid_index+1, leaf.songAtIndex(i));
			}
		}
		else{
			for(int i = 0; i < mid_index; i++) {
				left_node.addSong(i, leaf.songAtIndex(i));
			}

			int size = leaf.songCount();
			for(int i = mid_index; i < size; i++) {
				right_node.addSong(i - mid_index, leaf.songAtIndex(i));
			}
		}


		if(parent == null){ //for root
			parent = new PlaylistNodePrimaryIndex(null);
			left_node.setParent(parent);
			right_node.setParent(parent);
			parent.set_child(0, left_node);
			parent.set_child(1, right_node);
			if(mode == 0){
				parent.set_value(0, song.audioId());
			} else {
				parent.set_value(0, right_node.audioIdAtIndex(0));
			}

		}
		else { //id there are middle nodes
			int pos;
			// if newly added song goes up
			if (mode == 0){
				pos = parent.get_position(song.audioId());
				parent.set_value(pos, song.audioId());
			}
			// if newly added song doesn't go up
			else {
				pos = parent.get_position(mid_value);
				parent.set_value(pos, right_node.audioIdAtIndex(0));
			}

			parent.set_child(pos+1, right_node);
			parent.update_child(pos, left_node);

		}
		if (parent.getParent() == null){
			primaryRoot = parent;
		}
		add_to_primary(song);

		return parent;
	}

	public PlaylistNodePrimaryIndex p_p_up(PlaylistNodePrimaryIndex index_node) {
		int mid_index = index_node.audioIdCount()/2;
		int mid_value = index_node.audioIdAtIndex(mid_index);
		PlaylistNodePrimaryIndex parent = (PlaylistNodePrimaryIndex) index_node.getParent();

		PlaylistNode node;

		PlaylistNodePrimaryIndex left_node = new PlaylistNodePrimaryIndex(parent);
		for (int i = 0; i < mid_index; i++) {
			node = index_node.getChildrenAt(i);
			node.setParent(left_node);
			left_node.set_child(i, node);
			left_node.set_value(i, index_node.audioIdAtIndex(i));
		}

		node = index_node.getChildrenAt(mid_index);
		node.setParent(left_node);
		left_node.set_child(mid_index, node);

		PlaylistNodePrimaryIndex right_node = new PlaylistNodePrimaryIndex(parent);
		for (int i = mid_index + 1 ; i < index_node.audioIdCount(); i++) {
			node = index_node.getChildrenAt(i);
			node.setParent(right_node);
			right_node.set_child(i - mid_index - 1, node);
			right_node.set_value(i - mid_index - 1, index_node.audioIdAtIndex(i));
		}

		node = index_node.getChildrenAt(index_node.audioIdCount());
		node.setParent(right_node);
		right_node.set_child(index_node.audioIdCount()- mid_index - 1 , node);

		if(parent == null){
			parent = new PlaylistNodePrimaryIndex(null);
			left_node.setParent(parent);
			right_node.setParent(parent);
			parent.set_child(0, left_node);
			parent.set_child(1, right_node);
			parent.set_value(0, mid_value);
		}
		else{
			int pos = parent.get_position(mid_value);
			parent.set_value(pos, mid_value);
			parent.set_child(pos+1, right_node);
			parent.update_child(pos, left_node);
		}
		return parent;
	}

	public void searchSong(Integer audioId) {
		// TODO: Implement this method
		// find the song with the searched audioId in primary B+ tree
		// return value will not be tested, just print according to the specifications

		ArrayList<PlaylistNode> visited = new ArrayList<>();
		PlaylistNode tmp = primaryRoot;
		while (tmp.getType() != PlaylistNodeType.Leaf){
			visited.add(tmp);
			int i = ((PlaylistNodePrimaryIndex)tmp).get_place(audioId);
			tmp = ((PlaylistNodePrimaryIndex)tmp).getChildrenAt(i);
		}
		int pos=0;
		while ( pos < ((PlaylistNodePrimaryLeaf)tmp).songCount() && !audioId.equals(((PlaylistNodePrimaryLeaf) tmp).audioIdAtIndex(pos))){
			pos += 1;
		}


		PlaylistNodePrimaryIndex visited_middle;
		int count = 0;
		String indent;
		for(int i = 0; i < visited.size(); i++){
			visited_middle = (PlaylistNodePrimaryIndex) visited.get(i);
			indent = "	".repeat(i);
			System.out.println(indent + "<index>");
			for (int j = 0; j < visited_middle.audioIdCount(); j++){
				System.out.println(indent + visited_middle.audioIdAtIndex(j));
			}
			System.out.println(indent + "</index>");
			count = i+1;
		}

		if (pos == ((PlaylistNodePrimaryLeaf)tmp).songCount()){
			System.out.println("Could not find " + audioId);
		}
		else{
			indent = "	".repeat(count);
			CengSong song = ((PlaylistNodePrimaryLeaf) tmp).songAtIndex(pos);
			System.out.println(indent + "<data>");
			System.out.println(indent + "<record>" + song.audioId() + "|" + song.genre() + "|" + song.songName() + "|" + song.artist() + "</record>");
			System.out.println(indent + "</data>");
		}

	}

	public void printPrimaryPlaylist() {
		// TODO: Implement this method
		// print the primary B+ tree in Depth-first order
		p_depth_first_travel(primaryRoot , 0);
	}

	public void p_depth_first_travel(PlaylistNode root, int indent_count) {
		String indent = "	".repeat(indent_count);
		if (root.getType() == PlaylistNodeType.Internal) {

			PlaylistNodePrimaryIndex tmp = ((PlaylistNodePrimaryIndex) root);

			System.out.println(indent + "<index>");
			for (int i = 0; i < tmp.audioIdCount(); i++) {
				System.out.println(indent + tmp.audioIdAtIndex(i));
			}
			System.out.println(indent + "</index>");

			int count = ((PlaylistNodePrimaryIndex) root).audioIdCount();
			for (int i = 0; i <= count; i++) {
				p_depth_first_travel(((PlaylistNodePrimaryIndex) root).getChildrenAt(i), indent_count + 1);
			}
		}
		else if (root.getType() == PlaylistNodeType.Leaf) {
			int count = ((PlaylistNodePrimaryLeaf) root).songCount();
			CengSong song;

			System.out.println(indent + "<data>");
			for (int i = 0; i < count; i++) {
				song = ((PlaylistNodePrimaryLeaf) root).songAtIndex(i);
				System.out.println(indent + "<record>" + song.audioId() + "|" + song.genre() + "|" + song.songName() + "|" + song.artist() + "</record>");
			}
			System.out.println(indent + "</data>");

		}
	}
	public void printSecondaryPlaylist() {
		// TODO: Implement this method
		// print the secondary B+ tree in Depth-first order
		s_depth_first_travel(secondaryRoot, 0);
		return;
	}
	public void s_depth_first_travel(PlaylistNode root, int indent_count) {
		String indent = "	".repeat(indent_count);
		String record_indent = "	".repeat(indent_count+1);
		if (root.getType() == PlaylistNodeType.Internal) {

			PlaylistNodeSecondaryIndex tmp = ((PlaylistNodeSecondaryIndex) root);

			System.out.println(indent + "<index>");
			for (int i = 0; i < tmp.genreCount(); i++) {
				System.out.println(indent + tmp.genreAtIndex(i));
			}
			System.out.println(indent + "</index>");

			int count = ((PlaylistNodeSecondaryIndex) root).genreCount();
			for (int i = 0; i <= count; i++) {
				s_depth_first_travel(((PlaylistNodeSecondaryIndex) root).getChildrenAt(i), indent_count + 1);
			}
		}
		else if (root.getType() == PlaylistNodeType.Leaf) {
			int count = ((PlaylistNodeSecondaryLeaf) root).genreCount();

			String genre;
			ArrayList<CengSong> songs ;
			System.out.println(indent + "<data>");
			for (int i = 0; i < count; i++) {
				songs = ((PlaylistNodeSecondaryLeaf) root).songsAtIndex(i);
				genre = ((PlaylistNodeSecondaryLeaf) root).genreAtIndex(i);
				System.out.println(indent + genre);
				for (int j = 0; j < songs.size(); j++) {
					System.out.println(record_indent + "<record>" + songs.get(j).audioId() + "|" + songs.get(j).genre() + "|" + songs.get(j).songName() + "|" + songs.get(j).artist() + "</record>");
				}
			}
			System.out.println(indent + "</data>");

		}
	}
	// Extra functions if needed

}


